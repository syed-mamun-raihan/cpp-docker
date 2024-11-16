#include <iostream>     // std::cout
#include <algorithm>    // std::rotate_copy
#include <vector>       // std::vector
// variant example
#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

template<typename T>
std::ostream& print(std::ostream &out, T const &val) { return (out << val << " "); }

template<typename T1, typename T2>
std::ostream& print(std::ostream &out, std::pair<T1, T2> const &val) { return (out << "{" << val.first << " " << val.second << "} "); }

template<template<typename, typename...> class TT, typename... Args>
std::ostream& operator<<(std::ostream &out, TT<Args...> const &cont) {
  for(auto elem : cont) print(out, elem);
  return out;
}

template <class ForwardIterator>
void rotate (ForwardIterator first, ForwardIterator middle,
               ForwardIterator last)
{
  ForwardIterator next = middle;
  while (first!=next)
  {
    std::swap (*first++,*next++);
    if (next==last) next=middle;
    else if (first==middle) middle=next;
  }
}

//-----------------
// LRU
class LRUCache{
    public:
    LRUCache(int capacity_)
    {
        capacity = capacity_;
    }

    void add(int key, string data)
    {
        if(db.size() == capacity)
        {
            auto it = LRUstates.begin();     
            db.erase(it->second);
            LRUstates.erase(it);
        }
        auto now = time(0);

        auto [it, success] = db.emplace(key, {data, now});
        if(success)
        {
            LRUstates.emplace(now, key);
        }        
    }

    std::string get(int key) const
    {
        auto& [data, now] = db[key];
        time_t nowUpdate = time(0);
        auto it = LRUStates.equal_range(now);
        for(auto exact = it.first; exact != it.second; exact++)
        {
            if(exact->second == key)
            {
                LRUStates.erase(exact);
            }
        }
        LRUstates[nowUpdate] = key;
        now = nowUpdate;
        return data;
    }

    void update(int key, std::string data);
    void erase(int key);

    private:
    int capacity = 10;
    using cache_db = std::unordered_map<int, pair<string, time_t>>;
    cache_db db;
    std::multimap<time_t, int> LRUstates;
};


 
// the variant to visit
using var_t = std::variant<int, long, double, std::string>;
 
// helper type for the visitor #4
template<class... Ts>
struct overload : Ts... 
{ 
    using Ts::operator()...; 
};

int main()
{
    std::vector<var_t> vec = {10, 15l, 1.5, "hello"};
    for (auto& v: vec)
    {
        // 4. another type-matching visitor: a class with 3 overloaded operator()'s
        // Note: The `(auto arg)` template operator() will bind to `int` and `long`
        //       in this case, but in its absence the `(double arg)` operator()
        //       *will also* bind to `int` and `long` because both are implicitly
        //       convertible to double. When using this form, care has to be taken
        //       that implicit conversions are handled correctly.
        std::visit(overload {
            [](auto arg) { std::cout << arg << ' '; },
            [](double arg) { std::cout << std::fixed << arg << ' '; },
            [](const std::string& arg) { std::cout << std::quoted(arg) << ' '; }
        }, v);
    }
}