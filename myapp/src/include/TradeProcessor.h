#pragma once

#include <bits/stdc++.h>

using std::operator""s;

namespace Trade{
    
struct TradeInputKeys
{
    std::string type; // N,C,F
    std::string symbol;
    std::string side;  // B, S
    std::string to_string() const
    {
        std::ostringstream oss;
        oss << " type: " << type << " symbol: " << symbol << " side: " << side;
        return oss.str();
    }
 };

struct TradeInputValues
{
    int         userId = 0;
    int         userOrderId = 0;
    int         qty = 0;
    int         price = 0;
    bool        priceParsed = false;
    std::string to_string() const
    {
        std::ostringstream oss;
        oss << " userId: " << userId << " userOrderId: " << userOrderId << " qty: " << qty << " price: " << price;
        return oss.str();
    }
};

class TradeProcessor
{
    public:
        TradeProcessor() = default;
        bool execute(const TradeInputKeys& key, TradeInputValues& value);

        using Orders = std::vector<TradeInputValues>;
        using OrderBook = std::map<std::string /*B, S*/, Orders>;
        using Books = std::unordered_map< std::string /*Symbol*/, OrderBook>;

    private:
        bool LimitBuyOrder(TradeInputValues& value, Orders& sellOrders);
        bool LimitSellOrder(TradeInputValues& value, Orders& buyOrders);

        bool MarketBuyOrder(TradeInputValues& value, Orders& sellOrders);
        bool MarketSellOrder(TradeInputValues& value, Orders& buyOrders);

        bool ClearOrder(const TradeInputValues& value);
        bool Flush();

    private:
        Books books;
        std::mutex mutex_;
}; 

}