#include "include/TradeProcessor.h"

using namespace Trade;

/**
 * match buy to sell limit using price-time priority
 */
bool TradeProcessor::LimitBuyOrder(TradeInputValues& value, Orders& sellOrders)
{
    bool tobChanged = false;
    for(auto it = sellOrders.begin(); it != sellOrders.end(); it++)
    {
        auto& sell = *it;
        if(sell.price == value.price)
        {
            tobChanged = true;
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
    return tobChanged;
}

/**
 * match sell to buy limit using price-time priority
 */
bool TradeProcessor::LimitSellOrder(TradeInputValues& value, Orders& buyOrders)
{
    bool tobChanged = false;
    for(auto it = buyOrders.begin(); it != buyOrders.end(); it++)
    {
        auto& buy = *it;
        if(buy.price == value.price)
        {
            tobChanged = true;
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
    return tobChanged;
}

/**
 * match Buy to Sell market Order
 */
bool TradeProcessor::MarketBuyOrder(TradeInputValues& value, Orders& sellOrders)
{
    bool tobChanged = false;
    for(auto it = sellOrders.begin(); it != sellOrders.end(); it++)
    {
        tobChanged = true;
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
    return tobChanged;
}

/**
 * match sell to buy market Order
 */
bool TradeProcessor::MarketSellOrder(TradeInputValues& value, Orders& buyOrders)
{
    bool tobChanged = false;
    for(auto it = buyOrders.begin(); it != buyOrders.end(); it++)
    {
        tobChanged = true;
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
    return tobChanged;
}

bool TradeProcessor::ClearOrder(const TradeInputValues& value)
{
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
        std::cout << "C, " << value.userId <<", " << value.userOrderId << "\n";
    }
    return false; // dont compute tob for all books for now
}

/**
 * Flush the book
 */
bool TradeProcessor::Flush()
{
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
    return false; // dont compute tob for all books
}

/** 
 * Execute a given Trade defined by Key, Value
 * 
 */
bool TradeProcessor::execute(const TradeInputKeys& key, TradeInputValues& value)
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
            LimitBuyOrder(value, sellOrders);
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
            LimitSellOrder(value, buyOrders);
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
            MarketBuyOrder(value, sellOrders);
        }
        else if(key.side == "S")
        {
            std::lock_guard lock(mutex_);
            // Try to match sell for the market order
            MarketSellOrder(value, buyOrders);
        }
        else
        {
            return false; // not Buy or Sell with New Order
        }
    }
    else if(key.type == "C")
    {
        std::lock_guard lock(mutex_);
        ClearOrder(value);
    }
    else if(key.type == "F") // Flush the books
    {
        std::lock_guard lock(mutex_);
        Flush();
    }
    return true;
}
