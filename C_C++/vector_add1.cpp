#include <iostream>
#include <vector>
#include <map>
#include <stack>

/*
    给定一个由整数组成的非空数组所表示的非负整数，在该数的基础上加一。
 */
using namespace std;

class solution
{
public:
    vector<int> plusOne(vector<int>& digits)
    {
        for( int i = digits.size()-1; i >= 0; i--)
        {
            digits[i]++;
            if( digits[i] != 10 )
                return digits;
            else
            {
                digits[i] = 0;
            }
        }
        digits.insert(digits.begin(), 1);
        return digits;
    }
};

int main()
{
    vector<int> num{1, 2, 9};
    class solution test;
    vector<int> out = test.plusOne(num);

    int len = out.size();
    for( int i = 0; i < len; i++ )
        cout << out[i];
    cout << endl;
    while(1);
    return 0;
}