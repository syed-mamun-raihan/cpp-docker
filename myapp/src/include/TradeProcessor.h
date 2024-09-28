#pragma once

#include <bits/stdc++.h>

using std::operator""s;

namespace Trade{
    
/// @brief captures Trade keys
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

/// @brief Captures Trade Values
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

/// @brief Implements trade processing functionalities
class TradeProcessor
{
    public:
        TradeProcessor() = default;
        bool execute(const TradeInputKeys& key, TradeInputValues& value);

        using Orders = std::vector<TradeInputValues>;
        using OrderBook = std::map<std::string /*B, S*/, Orders>;
        using Books = std::unordered_map< std::string /*Symbol*/, OrderBook>;

        using tobRecord = std::pair<int /*price*/, int /*qty*/>;
        using tobBook = std::map<std::string /*B, S*/, tobRecord>;
        using tobBooks = std::unordered_map< std::string /*Symbol*/, tobBook>;
        std::recursive_mutex output_mutex;

    private:
        bool LimitBuyOrder(TradeInputValues& value, Orders& sellOrders, tobRecord& tob);
        bool LimitSellOrder(TradeInputValues& value, Orders& buyOrders, tobRecord& tob);

        bool MarketBuyOrder(TradeInputValues& value, Orders& sellOrders, tobRecord& tob);
        bool MarketSellOrder(TradeInputValues& value, Orders& buyOrders, tobRecord& tob);

        bool ClearOrder(const TradeInputValues& value);
        bool Flush();

        void writeTOB(const Orders& order, const std::string& type, tobRecord& tob);

    private:
        Books books;
        tobBooks tob_books;
        std::recursive_mutex mutex;
}; 

}