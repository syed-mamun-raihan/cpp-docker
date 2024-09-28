#include "include/TradeProcessor.h"

using namespace Trade;

bool TraderProcessor::execute(const TradeInputKeys& key, TradeInputValues& value)
{ 
    if(value.price != 0 && key.type == "N") // limit
    {
        auto& book = books[key.symbol];
        auto& buyOrders = book["B"s];
        auto& sellOrders = book["S"s];
        if(key.side == "B")
        {
            std::lock_guard lock(mutex_);
            // Try to match sell for the limit order
            for(auto it = sellOrders.begin(); it != sellOrders.end(); it++)
            {
                auto& sell = *it;
                if(sell.price == value.price)
                {
                    if(sell.qty >= value.qty)
                    {
                        sell.qty -= value.qty;
                        // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                        std::cout << "T, " << value.userId <<", " << value.userOrderId << ", " << sell.userId << ", " << sell.userOrderId << ", " << sell.price << ", " << value.qty << std::endl;
                        if(sell.qty == 0)
                        {
                            it = sellOrders.erase(it);
                        }
                        value.qty = 0;
                        break; // market buy complete
                    }
                    else
                    {
                        value.qty -= sell.qty;
                        // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                        std::cout << "T, " << value.userId <<", " << value.userOrderId << ", " << sell.userId << ", " << sell.userOrderId << ", " << sell.price << ", " << sell.qty << std::endl;
                        it = sellOrders.erase(it);
                    } 
                }
            }
            if(value.qty)
            {
                buyOrders.emplace_back(value);
                std::cout << "A, " << value.userId <<", " << value.userOrderId << std::endl;
            }
        }
        else if(key.side == "S")
        {
            std::lock_guard lock(mutex_);
            // Try to match sell for the limit order
            for(auto it = buyOrders.begin(); it != buyOrders.end(); it++)
            {
                auto& buy = *it;
                if(buy.price == value.price)
                {
                    if(buy.qty >= value.qty)
                    {
                        buy.qty -= value.qty;
                        // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                        std::cout << "T, " << buy.userId <<", " << buy.userOrderId << ", " << value.userId << ", " << value.userOrderId << ", " << buy.price << ", " << value.qty << std::endl;
                        if(buy.qty == 0)
                        {
                            it = buyOrders.erase(it);
                        }
                        value.qty = 0;
                        break; // market buy complete
                    }
                    else
                    {
                        value.qty -= buy.qty;
                        // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                        std::cout << "T, " << buy.userId <<", " << buy.userOrderId << ", " << value.userId << ", " << value.userOrderId << ", " << buy.price << ", " << buy.qty << std::endl;
                        it = buyOrders.erase(it);
                    } 
                }
            }
            if(value.qty)
            {
                sellOrders.emplace_back(value);
                std::cout << "A, " << value.userId <<", " << value.userOrderId << std::endl;
            }
        }
        else
        {
            return false; // invalid side
        }
    }
    else if (value.price == 0 && key.type == "N")// market
    {
        auto& book = books[key.symbol];
        auto& buyOrders = book["B"s];
        auto& sellOrders = book["S"s];
        if(key.side == "B")
        {
            std::lock_guard lock(mutex_);
            // match sell - earliest time first
            for(auto it = sellOrders.begin(); it != sellOrders.end(); it++)
            {
                auto& sell = *it;
                if(sell.qty >= value.qty)
                {
                    sell.qty -= value.qty;
                    // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                    std::cout << "T, " << value.userId <<", " << value.userOrderId << ", " << sell.userId << ", " << sell.userOrderId << ", " << sell.price << ", " << value.qty << std::endl;
                    if(sell.qty == 0)
                    {
                        it = sellOrders.erase(it);
                    }
                    break; // market buy complete
                }
                else
                {
                    value.qty -= sell.qty;
                    // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                    std::cout << "T, " << value.userId <<", " << value.userOrderId << ", " << sell.userId << ", " << sell.userOrderId << ", " << sell.price << ", " << sell.qty << std::endl;
                    it = sellOrders.erase(it);
                }
            }
        }
        else if(key.side == "S")
        {
            // Try to match sell for the limit order
            for(auto it = buyOrders.begin(); it != buyOrders.end(); it++)
            {
                auto& buy = *it;
                if(buy.qty >= value.qty)
                {
                    buy.qty -= value.qty;
                    // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                    std::cout << "T, " << buy.userId <<", " << buy.userOrderId << ", " << value.userId << ", " << value.userOrderId << ", " << buy.price << ", " << value.qty << std::endl;
                    if(buy.qty == 0)
                    {
                        it = buyOrders.erase(it);
                    }
                    break; // market buy complete
                }
                else
                {
                    value.qty -= buy.qty;
                    // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                    std::cout << "T, " << buy.userId <<", " << buy.userOrderId << ", " << value.userId << ", " << value.userOrderId << ", " << buy.price << ", " << buy.qty << std::endl;
                    it = buyOrders.erase(it);
                } 
            }
        }
        else
        {
            return false; // not Buy or Sell with New Order
        }
    }
    else if(key.type == "C")
    {
        std::lock_guard lock(mutex_);
        bool cleared = false;
        for(auto& [symbol, book] : books)
        {
            for( auto& [type, bookBuySell] : book)
            {
                for(auto it = bookBuySell.begin(); it != bookBuySell.end(); it++)
                {
                    if(value.userId == it->userId && value.userOrderId == it->userOrderId)
                    {
                        bookBuySell.erase(it);
                        cleared = true;
                        break;
                    }
                }
                if(cleared)
                    break;
            }
            if(cleared)
                break;
        }
        if(cleared)
        {
            std::cout << "C, " << value.userId <<", " << value.userOrderId << "\n"
        }
    }
    else if(key.type == "F") // Flush the books
    {
        std::lock_guard lock(mutex_);
        for(auto& [symbol, book] : books)
        {
            auto& buyOrders = book["B"s];
            auto& sellOrders = book["S"s];
            for(auto itSell = sellOrders.begin(); itSell != sellOrders.end(); itSell++)
            {
                for(auto itBuy = buyOrders.begin(); itBuy != buyOrders.end(); itBuy++)
                {
                    if(itSell->price == itBuy->price) 
                    {
                        if(itSell->qty >= itBuy->qty)
                        {
                            itSell->qty -= itBuy->qty;
                        // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                            std::cout << "T, " << itBuy->userId <<", " << itBuy->userOrderId << ", " << itSell->userId << ", " << itSell->userOrderId << ", " << itSell->price << ", " << itBuy->qty << std::endl;
                            itBuy = buyOrders.erase(itBuy);
                        }
                        else
                        {
                            itBuy->qty -= itSell->qty;
                    // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                            std::cout << "T, " << itBuy->userId <<", " << itBuy->userOrderId << ", " << itSell->userId << ", " << itSell->userOrderId << ", " << itSell->price << ", " << itSell->qty << std::endl;
                            itSell = buyOrders.erase(itSell);
                        }
                    }
                }
            }
        }
    }
    return true;
}
