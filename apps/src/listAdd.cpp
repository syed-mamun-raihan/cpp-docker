#include <list>
#include <iostream>

//list<int> x = {2,4,3}; ==> 342
//list<int> y= {5,6,4}; ==> 465
//-------------------
//list<int> z = {7,0,8} ==> 807
using namespace std;

list<int> addNumbers(list<int> &x, list<int>& y)
{
    int currSum = 0;
    int carry = 0;
    auto itX = x.begin(); //2
    auto itY = y.begin(); // 5
    auto xEnd = x.end();
    auto yEnd = y.end();
    list<int> result;
    cout << " main loop\n";
    while(itX != xEnd && itY != yEnd)
    {   
        currSum = *itX + *itY + carry;
        carry = currSum / 10;
        currSum = currSum % 10;
        result.push_back (currSum);
        itX++;
        itY++;
    } 
    
    cout << " X loop\n";
    while(itX != xEnd)
    {
        if(carry)
        {
            currSum = *itX + carry;
            carry = currSum / 10;
            currSum = currSum % 10;
            result.push_back(currSum);
        }
        else
        {
            result.push_back(*itX);
        }
        itX++;
    }
    
    cout << " Y loop\n";
    while(itY != yEnd)
    {
        if(carry)
        {
            currSum = *itY + carry;
            carry = currSum / 10;
            currSum = currSum % 10;
            result.push_back(currSum);
        }
        else
        {
            result.push_back(*itY);
        }
        itY++;
    }
    
    cout << " Print loop\n";
    for(auto i : result)
    {
        cout << i;
    }
    cout << endl;
    return result;
}

int main(int , char** )
{
 list<int> x = {2,4,3};
 list<int> y= {5,6,4};
 auto result = addNumbers(x, y); 
 return 0;
}