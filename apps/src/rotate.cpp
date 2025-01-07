#include <bits/stdc++.h>
using namespace std;
 
/* Link list node */
class Node {
public:
    int data;
    Node* next;
};
 
/* A utility function to push a node */
void push(Node** head_ref, int new_data)
{
    /* allocate node */
    Node* new_node = new Node();
 
    /* put in the data */
    new_node->data = new_data;
 
    /* link the old list of the new node */
    new_node->next = (*head_ref);
 
    /* move the head to point to the new node */
    (*head_ref) = new_node;
}
 
/* A utility function to print linked list */
void printList(Node* node)
{
    while (node != NULL) {
        cout << node->data << " -> ";
        node = node->next;
    }
    cout << "NULL";
}
 
class Node {
  public:
    int data;
    Node *next;

    Node(int new_data) {
        data = new_data;
        next = nullptr;
    }
};

// Function to rotate the linked list
// to the left by k places
Node *rotate(Node *head, int k) {

    // If the linked list is empty or no rotations are
    // needed, then return the original linked list
    if (k == 0 || head == nullptr)
        return head;

    Node *curr = head;
    int len = 1;
  
      // Find the length of linked list
    while (curr->next != nullptr) {
        curr = curr->next;
        len += 1;
    }

      // Modulo k with length of linked list to handle
      // large values of k
    k %= len;
  
    if (k == 0)
        return head;
  
      // Make the linked list circular
    curr->next = head;

      // Traverse the linked list to find the kth node
    curr = head;
    for (int i = 1; i < k; i++)
        curr = curr->next;

      // Update the (k + 1)th node as the new head
    head = curr->next;
  
      // Break the loop by updating next pointer of kth node
    curr->next = nullptr;
    return head;
}
//
class Node {
public:
    int data;
    Node* next;

    Node(int new_data) {
        data = new_data;
        next = nullptr;
    }
};

// Given the head of a list, reverse the list and return the
// head of reversed list
Node* reverseList(Node* head) {
    
      // Initialize three pointers: curr, prev and next
    Node *curr = head, *prev = nullptr, *next;

      // Traverse all the nodes of Linked List
    while (curr != nullptr) {
      
        // Store next
        next = curr->next;
      
        // Reverse current node's next pointer
        curr->next = prev;
      
        // Move pointers one position ahead
        prev = curr;
        curr = next;
    }
      
      // Return the head of reversed linked list
    return prev;
}

// Print container
template<typename T>
std::ostream& print(std::ostream &out, T const &val) { return (out << val << " "); }

template<typename T1, typename T2>
std::ostream& print(std::ostream &out, std::pair<T1, T2> const &val) { return (out << "{" << val.first << " " << val.second << "} "); }

template<template<typename, typename...> class TT, typename... Args>
std::ostream& operator<<(std::ostream &out, TT<Args...> const &cont) {
  for(auto elem : cont) print(out, elem);
  return out;
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

template <typename T>
concept Number = (std::integral<T> || std::floating_point<T>) 
                 && !std::same_as<T, bool>
                 && !std::same_as<T, char>
                 && !std::same_as<T, char8_t>
                 && !std::same_as<T, char16_t>
                 && !std::same_as<T, char32_t>
                 && !std::same_as<T, wchar_t>;

template <typename BusinessObjectWithEncodeableStuff_t>
concept BusinessObjectWithEncodeableStuff = requires (BusinessObjectWithEncodeableStuff_t bo) {
  bo.interfaceA();
  bo.interfaceB();
  { bo.interfaceC() } -> std::same_as<int>;
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