#include <bits/stdc++.h>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "include/TradeProcessor.h"

using std::operator""s;

auto trim = [](const std::string_view& str,
                 const std::string& whitespace = " \t") -> std::string
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return std::ranges::to<std::string>(str.substr(strBegin, strRange));
};

///////
using boost::asio::ip::udp;

namespace Trade{

/**
 * Main Application class 
 * Opens a UDP Port to listen
 * Parse trade Request
 * Execute Market Order Immediately (Fill and Kill)
 * Execute Limit Order  using price-time matches
 */
class TraderApp {
public:
    TraderApp(boost::asio::io_service& io_service, std::size_t n_threads)
        : socket(io_service, udp::endpoint(udp::v4(), 1234))
    {
        for (std::size_t i = 0; i < n_threads; ++i)
        {
            threads.emplace_back(make_thread_handler());
        }
        startReceive();
    }

private:
    std::jthread make_thread_handler()
    {
        return std::jthread{
            [this](std::stop_token stoken){
                bool scenerioFound = false;
                while (!stoken.stop_requested())
                {
                    std::string line = trim(pop());
                    if(line.empty())
                    {
                        continue;
                    }
                    
                    if(line.contains('#'))
                    {
                        const auto searchItem = "name:scenario"s;
                        if(line.contains(searchItem))
                        {
                            const auto searchItemPos = line.find(searchItem);
                            const auto searchItemLength = searchItem.length();
                            const auto lineLength = line.length();
                            const auto scenarioIdStr = line.substr(searchItemPos+searchItemLength, lineLength-1);
                            try{
                                int scenarioId = stoi(scenarioIdStr);
                                if( scenarioId % 2 == 0)
                                {
                                    scenerioFound = true;
                                    std::osyncstream out(std::cout);
                                    out << "#name: scenario " << scenarioId << std::endl;
                                }
                                else
                                {
                                    scenerioFound = false;
                                }
                            }catch(std::exception& e)
                            {
                                std::osyncstream out(std::cerr);
                                out << line << std::endl;
                                out << " searchItemPos: " << searchItemPos << " searchItemLength " << searchItemLength << " lineLength " << lineLength << std::endl;
                                out << " scenarioIdStr " << scenarioIdStr << std::endl;
                                out << e.what();
                                out.flush();
                            }
                        }
                        continue;
                    }

                    if(!scenerioFound)
                        continue;

                    TradeInputKeys key;
                    TradeInputValues value;
                    bool success = false;
                    std::istringstream iss(line);
                    for (std::string word; std::getline(iss, word, ',');)
                    {
                        word = trim(word);
                        if(key.type.empty()) // Order Type
                        {
                            if(word == "N" || word == "C" || word == "F")
                            {
                                key.type = word;
                            }
                            else
                            {
                                break;
                            }
                            if(key.type == "F")
                            {
                                success = true;
                                break;
                            }
                            continue;
                        }
                        if(value.userId == 0)
                        {
                            value.userId = std::stoi(word);
                            if(value.userId == 0)
                            {
                                break;
                            }
                            continue;
                        }
                        if(key.type == "N" && key.symbol.empty())
                        {
                            key.symbol = word;
                            if(key.symbol.empty())
                            {
                                break;
                            }
                            continue;
                        }
                        if(key.type == "C" && value.userOrderId == 0)
                        {
                            value.userOrderId = std::stoi(word);
                            if(value.userOrderId == 0)
                            {
                                break;
                            }
                            success = true;
                            break;
                        }
                        if(key.type == "N" && !value.priceParsed)
                        {
                            value.price= std::stoi(word);
                            value.priceParsed = true;
                            continue;
                        }
                        if(key.type == "N" && value.qty == 0)
                        {
                            value.qty = std::stoi(word);
                            if(value.qty == 0) 
                            {
                                break;
                            }
                            continue;
                        }
                        if(key.type == "N" && key.side.empty())
                        {
                            if(word == "B" || word == "S")
                            {
                                key.side = word;
                            }
                            else
                            {
                                break;
                            }
                            continue;
                        }
                        if(key.type == "N" && value.userOrderId == 0)
                        {
                            value.userOrderId = std::stoi(word);
                            if(value.userOrderId == 0)
                            {
                                break;
                            }
                            success = true;
                            break;
                        }
                    }
                    if(success)
                    {
                        processor.execute(key, value);
                    }
                }
            } // end lambda
        };
    }

    /**
     * Wait for trade requests
     */
    void startReceive() {
        socket.async_receive_from(
            boost::asio::buffer(udpBuffer), remoteEndpoint,
            boost::bind(&TraderApp::handleReceive, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    /**
     * Handle trade requests
     */
    void handleReceive(const boost::system::error_code& error,
                       std::size_t bytes_transferred) {
        if (!error || error == boost::asio::error::message_size) 
        {
        //    std::lock_guard l(processor.output_mutex);
        //    std::cout << "# Got UDP data\n";
            // Parse data
            if(bytes_transferred)
            {
                std::istringstream iss(udpBuffer.data());
                for (std::string line; std::getline(iss, line);)
                {
                    push(line);
                }
            }
        }
        else
        {
            std::osyncstream out(std::cerr);
            out << "# handleReceive|Error: " << error << ", Retrying\n";
        }
        udpBuffer.fill('\0');
        startReceive();
    }
    
    /**
     * Save incoming requests
     */
    void push(std::string const& val)
    {
        std::lock_guard queue_lock{queue_mutex};
        strQueue.push(val);
        queue_cv.notify_one();
    }

    /**
     * Returns incoming requests for processing
     */
    std::string pop()
    {
        std::unique_lock<std::mutex> queue_lock{queue_mutex};
        queue_cv.wait(queue_lock, [&]{ return !strQueue.empty(); });
        auto ret = strQueue.front();
        strQueue.pop();
        return ret;
    }

private:
    /**
     * Store incoming requests
     */
    std::queue<std::string> strQueue;

    /**
     * For activating worker threads
     */
    std::condition_variable_any queue_cv;

    /**
     * Protect incoming trades in queue
     */
    std::mutex queue_mutex;

    /**
     * Worker Threads
     */
    std::vector<std::jthread> threads;

    /**
     * network socket to receive trades
     */
    udp::socket socket;

    /**
     * For handling incoming network connections
     */
    udp::endpoint remoteEndpoint;

    /**
     * Recieve Buffer
     */
    std::array<char, 16384> udpBuffer;

    TradeProcessor processor;
};
} // end namespace

int main()
{
   try {
        boost::asio::io_service io_service;
        Trade::TraderApp server{io_service, 1};
        io_service.run();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
 
    return 0; 
 }

