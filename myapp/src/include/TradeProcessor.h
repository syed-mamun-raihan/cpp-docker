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

class TraderProcessor
{
    public:
        TraderProcessor() = default;
        bool execute(const TradeInputKeys& key, TradeInputValues& value);

    private:
        using Orders = std::vector<TradeInputValues>;
        using OrderBook = std::map<std::string /*B, S*/, Orders>;
        std::unordered_map< std::string /*Symbol*/, OrderBook> books;
        std::mutex mutex_;
}; 

}