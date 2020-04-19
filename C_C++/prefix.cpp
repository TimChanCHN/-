#include <iostream>
#include <vector>
#include <map>

/*
    找出字符串数组的最长共同前缀
 */

using namespace std;

class solution {
public:
    string longestCommonPrefix(vector<string>& strs) {
        if( strs.size() == 0 )
            return "";
            
        for(int i = 0; i < strs[0].size(); i++ )
            for( int j = 1; j < strs.size(); j++ )
            {
                if( strs[j][i] != strs[0][i] )
                {
                    return strs[0].substr(0,i);
                }
            }
        return strs[0];
    }
};

int main()
{
    class solution test;
    vector<string> str{"aabdd", "a", "aabss"};
    string out = test.longestCommonPrefix(str);

    cout << out << endl;
    while(1);
    return 0;
}