#include <iostream>
#include <vector>

/*
    给出一个 32 位的有符号整数，你需要将这个整数中每位上的数字进行反转。
 */

using namespace std;

class solution
{
public:
    int reverse(int x)
    {
        int ans=0;

        while( x )
        {
            int tmp = x % 10;
            x /= 10;
            if( ans > INT_MAX/10 ) 
                return 0;
            if( ans < INT_MIN/10 )
                return 0;
            
            ans = ans*10 + tmp;
        }

        return ans;
    }
};

int main()
{
    class solution test;
    int out = test.reverse(123);

    cout << out << endl;
    while(1);
    return 0;
}