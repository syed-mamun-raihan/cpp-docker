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
template <class KEY_T, class VAL_T> class LRUCache{
private:
    list< pair<KEY_T,VAL_T> > item_list;
    unordered_map<KEY_T, decltype(item_list.begin()) > item_map;
    size_t cache_size;
private:
    void clean(void){
            while(item_map.size()>cache_size){
                    auto last_it = item_list.end(); last_it --;
                    item_map.erase(last_it->first);
                    item_list.pop_back();
            }
    }
public:
    LRUCache(int cache_size_):cache_size(cache_size_){
    }
    void put(const KEY_T &key, const VAL_T &val){
            auto it = item_map.find(key);
            if(it != item_map.end()){
                    item_list.erase(it->second); // iterartor of the list
                    item_map.erase(it); // iterator of the map
            }
            item_list.push_front(make_pair(key,val));
            item_map.insert(make_pair(key, item_list.begin()));
            clean();
    }

    bool exist(const KEY_T &key){
            return (item_map.count(key)>0);
    }
    VAL_T get(const KEY_T &key){
            assert(exist(key));
            auto it = item_map.find(key);
            item_list.splice(item_list.begin(), item_list, it->second); // put it in the front
            return it->second->second;
    }
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