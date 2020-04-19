#include <iostream>
#include <vector>
#include <map>
#include <stack>

/*
    外观序列：
    1： 1                   1
    2： 11                  1个1
    3： 21                  2个1
    4： 1211                1个2，2个1
 */
using namespace std;

class solution
{
public:
    string countAndSay(int n)
    {
        /* 模拟外观序列的生成过程 */
        string s = "1";

        for( int i = 1; i < n; i++ )
        {
            string tmp;
            for( int j = 0; j < s.size(); j++ )
            {
                int cnt = 1;
                while( j+1 < s.size() && (s[j+1] == s[j]) )
                {
                    cnt++;
                    j++;
                }
                tmp += std::to_string(cnt) + s[j];
            }
            s = tmp;
        }
        return s;
    }
};

int main()
{
    class solution test;
    string out = test.countAndSay(4);

    cout << out << endl;
    while(1);
    return 0;
}