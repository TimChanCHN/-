#include <iostream>

/*
    给定一个字符串 s，找到 s 中最长的回文子串。你可以假设 s 的最大长度为 1000。
    FIXME:找出回文在原字符串中的位序即可
 */

using namespace std;

class solution
{
public:
    /* 1. method1: 中心扩展法，从中心开始，再往两边扩展 */
    string huiwen(string s)
    {
        int begin = 0;
        int end = 0;

        for( int i = 0; i < s.length(); i++ )
        {
            /* aba型回文 */
            for( int l = i, r = i; l>=0 && r<s.length(); l--, r++ )
            {
                if( s[l] != s[r] )
                    break;
                else
                {
                    if( (r-l) > (end-begin) )
                    {
                        end = r;
                        begin = l;
                    }
                }
                
            }
            /* abba型回文 */
            for( int l = i, r = i+1; l>=0 && r<s.length(); l--,r++ )
            {
                if( s[r] == s[l] )
                {
                    if( (r-l) > (end-begin) )
                    {
                        end = r;
                        begin = l;
                    }
                }
                else
                {
                    break;
                }
            }
        }
        return s.substr(begin, end-begin+1);
    }

    /* 2. 动态规划 */
    string huiwen2(string s)
    {
        int len = s.length();
        int begin = 0;
        int end = 0;
        bool dp[len][len];            // 用于存储历史记录

        if( len == 0 )
            return s;

        for( int i = 0; i < len; i++ )
        {
            dp[i][i] = true;
        }

        for( int r = 1; r < len; r++ )
            for( int l = 0; l < r; l++ )
            {
                /* 如果上一循环是回文，且当前也是回文，则需要令对应的dp也为true */
                if( (s[l] == s[r]) && ((r-l==1)|| dp[r-1][l+1]))
                {
                    dp[r][l] = true;
                    if( (r-l) > (end-begin) )
                    {
                        end = r;
                        begin = l;
                    }
                    //continue;                                   /* 因为回文已经多了一位，则需要跳出该循环，令l和r都有移一位 */
                }
                else
                {
                    dp[r][l] = false;
                }
                
            }
        
        return s.substr(begin, end-begin+1);
    }
};

int main()
{
    class solution test;
    string str = test.huiwen("abaxx");
    string str1 = test.huiwen2("abcdcbamm");
    
    cout << "method1:" << str << endl;
    cout << "method2:" << str1 << endl;
    while(1);
    return 0;
}