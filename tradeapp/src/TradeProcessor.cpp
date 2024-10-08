#include "include/TradeProcessor.h"

using namespace Trade;

/**
 * match buy to sell limit using price-time priority
 */
bool TradeProcessor::LimitBuyOrder(TradeInputValues& value, Orders& sellOrders, tobRecord& tob)
{
    for(auto it = sellOrders.begin(); it != sellOrders.end(); it++)
    {
        auto& sell = *it;
        if(sell.price == value.price)
        {
            if(value.price == tob.first)
            {
                tob.first = 0; // reset TOB
            }

            if(sell.qty >= value.qty)
            {
                sell.qty -= value.qty;
                // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                std::lock_guard l(output_mutex);
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
                std::lock_guard l(output_mutex);
                std::cout << "T, " << value.userId <<", " << value.userOrderId << ", " << sell.userId << ", " << sell.userOrderId << ", " << sell.price << ", " << sell.qty << std::endl;
                it = sellOrders.erase(it);
            } 
        }
    } 
    return true;
}

/**
 * match sell to buy limit using price-time priority
 */
bool TradeProcessor::LimitSellOrder(TradeInputValues& value, Orders& buyOrders, tobRecord& tob)
{
    for(auto it = buyOrders.begin(); it != buyOrders.end(); it++)
    {
        auto& buy = *it;
        if(buy.price == value.price)
        {
            if(value.price == tob.first)
            {
                tob.first = 0; // reset TOB
            }
            if(buy.qty >= value.qty)
            {
                buy.qty -= value.qty;
                // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                std::lock_guard l(output_mutex);
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
                std::lock_guard l(output_mutex);
                std::cout << "T, " << buy.userId <<", " << buy.userOrderId << ", " << value.userId << ", " << value.userOrderId << ", " << buy.price << ", " << buy.qty << std::endl;
                it = buyOrders.erase(it);
            } 
        }
    }
    return true;
}

/**
 * match Buy to Sell market Order
 */
bool TradeProcessor::MarketBuyOrder(TradeInputValues& value, Orders& sellOrders, tobRecord& tob)
{
    for(auto it = sellOrders.begin(); it != sellOrders.end(); it++)
    {
        auto& sell = *it;
        if(sell.qty >= value.qty)
        {
            if(sell.price == tob.first)
            {
                tob.first = 0; // reset TOB
            }
            sell.qty -= value.qty;
            // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
            std::lock_guard l(output_mutex);
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
            std::lock_guard l(output_mutex);
            std::cout << "T, " << value.userId <<", " << value.userOrderId << ", " << sell.userId << ", " << sell.userOrderId << ", " << sell.price << ", " << sell.qty << std::endl;
            it = sellOrders.erase(it);
        }
    }
    return true;
}

/**
 * match sell to buy market Order
 */
bool TradeProcessor::MarketSellOrder(TradeInputValues& value, Orders& buyOrders, tobRecord& tob)
{
    for(auto it = buyOrders.begin(); it != buyOrders.end(); it++)
    {
        auto& buy = *it;
        if(buy.qty >= value.qty)
        {
            if(buy.price == tob.first)
            {
                tob.first = 0; // reset TOB
            }
            buy.qty -= value.qty;
            // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
            std::lock_guard l(output_mutex);
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
            std::lock_guard l(output_mutex);
            std::cout << "T, " << buy.userId <<", " << buy.userOrderId << ", " << value.userId << ", " << value.userOrderId << ", " << buy.price << ", " << buy.qty << std::endl;
            it = buyOrders.erase(it);
        } 
    }
    return true;
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
                    it = bookBuySell.erase(it);
                    cleared = true;
                    writeTOB(bookBuySell, type, tob_books[symbol][type]);
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
        std::lock_guard l(output_mutex);
        std::cout << "C, " << value.userId <<", " << value.userOrderId << "\n";
    }
    return true; 
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
                        std::lock_guard l(output_mutex);
                        std::cout << "T, " << itBuy->userId <<", " << itBuy->userOrderId << ", " << itSell->userId << ", " << itSell->userOrderId << ", " << itSell->price << ", " << itBuy->qty << std::endl;
                        itBuy = buyOrders.erase(itBuy);
                    }
                    else
                    {
                        itBuy->qty -= itSell->qty;
                // T, userIdBuy, userOrderIdBuy, userIdSell, userOrderIdSell, price, quantity
                        std::lock_guard l(output_mutex);
                        std::cout << "T, " << itBuy->userId <<", " << itBuy->userOrderId << ", " << itSell->userId << ", " << itSell->userOrderId << ", " << itSell->price << ", " << itSell->qty << std::endl;
                        itSell = buyOrders.erase(itSell);
                    }
                }
            }
        }
    }
    return false; // dont compute tob for all books
}

void TradeProcessor::writeTOB(const Orders& orders, const std::string& type, tobRecord& tob)
{
    bool changed = false;
    for(const auto & order : orders)
    {
        if(order.price > tob.first)
        {
            tob.first = order.price;
            tob.second = order.qty;
            changed = true;
        }
    }
    if(changed)
    {
        std::lock_guard l(output_mutex);
        std::cout << "B, " << type << ", " << tob.first << ", " << tob.second << std::endl;
    }
}

/** 
 * Execute a given Trade defined by Key, Value
 * 
 */
bool TradeProcessor::execute(const TradeInputKeys& key, TradeInputValues& value)
{ 
    std::lock_guard lock(mutex);
    if(value.price != 0 && key.type == "N") // limit
    {
        std::lock_guard l(output_mutex);
        std::cout << "A, " << value.userId <<", " << value.userOrderId << std::endl;
        auto& book = books[key.symbol];
        auto& buyOrders = book["B"s];
        auto& sellOrders = book["S"s];
        if(key.side == "B")
        {
            // Try to match sell for the limit order
            LimitBuyOrder(value, sellOrders, tob_books[key.symbol]["B"s]);
            if(value.qty)
            {
                buyOrders.emplace_back(value);
            }
            writeTOB(buyOrders, "B"s, tob_books[key.symbol]["B"s]);
        }
        else if(key.side == "S")
        {
            // Try to match sell for the limit order
            LimitSellOrder(value, buyOrders, tob_books[key.symbol]["S"s]);
            if(value.qty)
            {
                sellOrders.emplace_back(value);
            }
            writeTOB(sellOrders, "S"s, tob_books[key.symbol]["S"s]);
        }
        else
        {
            return false; // invalid side
        }
    }
    else if (value.price == 0 && key.type == "N")// market
    {
        std::lock_guard l(output_mutex);
        std::cout << "A, " << value.userId <<", " << value.userOrderId << std::endl;
        auto& book = books[key.symbol];
        auto& buyOrders = book["B"s];
        auto& sellOrders = book["S"s];
        if(key.side == "B")
        {
            // match sell - earliest time first
            MarketBuyOrder(value, sellOrders, tob_books[key.symbol]["S"s]);
            writeTOB(buyOrders, "S"s, tob_books[key.symbol]["S"s]);
        }
        else if(key.side == "S")
        {
            // Try to match sell for the market order
            MarketSellOrder(value, buyOrders, tob_books[key.symbol]["B"s]);
            writeTOB(sellOrders, "B"s, tob_books[key.symbol]["B"s]);
        }
        else
        {
            return false; // not Buy or Sell with New Order
        }
    }
    else if(key.type == "C")
    {
        ClearOrder(value);
    }
    else if(key.type == "F") // Flush the books
    {
        Flush();
    }
    return true;
}
