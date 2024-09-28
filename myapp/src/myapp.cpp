#include <bits/stdc++.h>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "include/TradeProcessor.h"

using std::operator""s;


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
        : _socket(io_service, udp::endpoint(udp::v4(), 1234))
    {
        std::cout << "# Worker threads count: " << n_threads << std::endl;
        for (std::size_t i = 0; i < n_threads; ++i)
        {
            _threads.push_back(make_thread_handler());
        }
        startReceive();
    }

private:
    std::jthread make_thread_handler()
    {
        return std::jthread{
            [this]{
                while (true)
                {
                    auto const line = pop();
                    line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
                    if(line.contains('#') || line.empty())
                    {
                        continue;
                    }
 
                    TradeInputKeys key;
                    TradeInputValues value;
                    bool success = false;
                    std::istringstream iss(line);
                    for (std::string word; std::getline(iss, word, ',');)
                    {
                        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
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
                        _processor.execute(key, value);
                    }
                }
            } // end lambda
        };
    }

    /**
     * Wait for trade requests
     */
    void startReceive() {
        _socket.async_receive_from(
            boost::asio::buffer(_recvBuffer), _remoteEndpoint,
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
            std::cout << "# Got UDP data\n";
            // Parse data
            if(bytes_transferred)
            {
                std::istringstream iss(_recvBuffer.data());
                for (std::string line; std::getline(iss, line);)
                {
                    push(line);
                }
            }
        }
        else
        {
            std::cout << "# handleReceive|Error: " << error << ", Retrying\n";
        }
        _recvBuffer.fill('\0');
        startReceive();
    }

    /**
     * Save incoming requests
     */
    void push(std::string const& val)
    {
        std::lock_guard<std::mutex> queue_lock{_queue_mutex};
        _queue.push(val);
        _queue_cv.notify_one();
    }

    /**
     * Returns incoming requests for processing
     */
    std::string pop()
    {
        std::unique_lock<std::mutex> queue_lock{_queue_mutex};
        _queue_cv.wait(queue_lock, [&]{ return !_queue.empty(); });
        auto ret = _queue.front();
        _queue.pop();
        return ret;
    }

private:
    /**
     * Store incoming requests
     */
    std::queue<std::string> _queue;

    /**
     * For activating worker threads
     */
    std::condition_variable _queue_cv;

    /**
     * Protect incoming trades in queue
     */
    std::mutex _queue_mutex;

    /**
     * Worker Threads
     */
    std::vector<std::jthread> _threads;

    /**
     * network socket to receive trades
     */
    udp::socket _socket;

    /**
     * For handling incoming network connections
     */
    udp::endpoint _remoteEndpoint;

    /**
     * Recieve Buffer
     */
    std::array<char, 16384> _recvBuffer;

    TraderProcessor _processor;
};
} // end namespace

int main()
{
   try {
        boost::asio::io_service io_service;
        Trade::TraderApp server{io_service, 4};
        io_service.run();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
 
    return 0; 
 }

