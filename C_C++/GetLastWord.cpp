#include <iostream>
#include <vector>
#include <map>
#include <stack>

/*
    给定一个仅包含大小写字母和空格 ' ' 的字符串 s，返回其最后一个单词的长度。如果字符串从左向右滚动显示，那么最后一个单词就是最后出现的单词。
 */
using namespace std;

class solution
{
public:
    int lengthOfLastWord(string s)
    {
        int len = s.size();
        int res = 0;

        for( int i = len-1; i >=0; i-- )
        {
            if( s[i] != ' ' )
                res++;
            else
            {
                if( res != 0 )
                    return res;
            }
        }
        return res;
    }
};

int main()
{
    vector<int> num{-2,1,-3,4,-1,2,1,-5,4};
    class solution test;
    int out = test.lengthOfLastWord("d ");

    cout << out << endl;
    while(1);
    return 0;
}