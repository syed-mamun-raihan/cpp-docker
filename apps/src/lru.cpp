#include <bits/stdc++.h>

class LRUCache {
public:
    struct Node{
        int key = INT_MIN;
        int value = INT_MIN;
        Node* next = nullptr, *prev = nullptr;
        Node() = default;
        Node(int key_, int value_) : key(key_), value(value_)
        {
        }
    };

    struct List
    {
        Node* head = new Node(); // sentinel object
        Node* tail = new Node(); // sentinel object

        List(){
            head->next = tail;
            tail->prev = head;
        }

        void erase(Node* node) // O(1)
        {
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }

        void push_front(Node* node) // O(1)
        {
            Node* temp = head->next;
            head->next = node;
            node->prev = head; 

            node->next = temp; // putback temp
            temp->prev = node;
        }

        Node* pop_back() // O(1)
        {
            Node* last = tail->prev;
            erase(last);
            return last;
        }
    };

    int capacity;
    unordered_map<int, Node*> mp;
    List lru;

    LRUCache(int capacity) {
        this->capacity = capacity;
    }
    
    int get(int key) {
        if(mp.find(key) != mp.end())
        {
            Node* currNode = mp[key];
            lru.erase(currNode);
            lru.push_front(currNode);
            return currNode->value;
        }
        return -1;
    }
    
    void put(int key, int value) {
        if(mp.find(key) != mp.end()){
            Node* currNode = mp[key];
            currNode->value = value; 
            lru.erase(currNode);
            lru.push_front(currNode);
        }
        else{
            Node* newNode = new Node(key, value);
            if(mp.size() == capacity)
            {
                Node* todel = lru.pop_back();
                mp.erase(todel->key);
                lru.push_front(newNode);
                mp[key]  = newNode;
                delete todel;
            }
            else{
                lru.push_front(newNode);
                mp[key] = newNode;
            }
        }
    }
};