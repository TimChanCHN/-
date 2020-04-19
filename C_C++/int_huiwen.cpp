#include <iostream>
#include <vector>

/*
    判断一个整数是否是回文数。回文数是指正序（从左向右）和倒序（从右向左）读都是一样的整数。
    方法：输出该整数的逆序值，如果两者相等，则说明是回文数
 */

using namespace std;

class solution
{
public:
    bool isPalindrome(int x)
    {
        int ans = 0;
        int tmp = x;

        /* 输出一个x的逆序整数 */
        if( x >= 0 )
        {
            while( tmp )
            {
                ans = ans*10 + tmp%10;
                tmp /= 10;
            }
        }
        return ans == x;
    }
};

int main()
{
    class solution test;
    bool out = test.isPalindrome(-123321);

    cout << out << endl;
    while(1);
    return 0;
}