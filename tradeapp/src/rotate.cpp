#include <iostream>     // std::cout
#include <algorithm>    // std::rotate_copy
#include <vector>       // std::vector

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