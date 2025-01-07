#include <bits/stdc++.h>

template <typename T>
class SharedPtr
{
    public:

        explicit SharedPtr(T* ptr = nullptr): ptr_(ptr)
        {
            ++SharedPtr::ref_counter;
        }

        ~SharedPtr()
        {
            --SharedPtr::ref_counter;
            if(SharedPtr::ref_counter <= 0)
            {
                delete ptr_;
            }
        }

        SharedPtr(const SharedPtr& other)
        {
            if(this == &other) return;
            ptr_ = other.ptr_;
            ++SharedPtr::ref_counter;
        }
        
        SharedPtr(SharedPtr&& other)
        {
            if(this == &other) return;
            ptr_ = other.ptr_;
            ref_counter = other.ref_counter;

            other.ptr_ = nullptr;
            other.ref_counter = 0;
        }

        SharedPtr& operator = (const T* ptr)
        {
            ptr_ = ptr;
            ++SharedPtr::ref_counter;
            return *this;
        }

        SharedPtr& operator = (const SharedPtr& other)
        {
            if(this == &other) return *this;
            if(ptr_ && SharedPtr::ref_counter <= 0) delete ptr_;

            ptr_ = other.ptr;
            ref_counter = other.ref_counter;
            return *this;
        }

        SharedPtr& operator = (SharedPtr&& other)
        {
            if(this == &other) return;
            ptr_ = other.ptr_;
            ref_counter = other.ref_counter;

            other.ptr_ = nullptr;
            other.ref_counter = 0;
            return *this;
        }

        T& operator * () 
        {
            return *ptr_;
        }

        const T& operator * () const
        {
            return *ptr_;
        }

        T* operator ->()
        {
            return ptr_;
        }

        const T* operator ->()const
        {
            return ptr_;
        }

        T* get()
        {
            return ptr_;
        }

        const T* get() const
        {
            return ptr_;
        }

        T* release()
        {
            T* temp = ptr_;
            ptr_ = nullptr;
            ref_counter = 0;
            return temp;
        }

    private:
        T* ptr_ = nullptr;
        static atomic<int> ref_counter = 0;
};

//
long long hash(string s)
{
    int pos = 0;
    long long hash = 0;
    for(char c : s)
    {
        hash = hash + c * pos;
        ++pos;
    }
}