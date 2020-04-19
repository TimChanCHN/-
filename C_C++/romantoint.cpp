#include <iostream>
#include <vector>
#include <map>

/*
        罗马数字转整数
 */

using namespace std;

class solution
{
public:
    int romanToInt(string s)
    {
        map<char,int> rom = {{'I',1}, {'V',5}, {'X',10}, {'L',50}, {'C',100}, {'D',500}, {'M',1000}};
        int num = 0;
        for( int i = 0; i < s.size(); i++ )
        {
            if( rom[s[i]] >= rom[s[i+1]] )
            {
                num = num + rom[s[i]];
            }
            else
            {
                num = rom[s[i+1]] - rom[s[i]] + num;
                i++;
            }
        }

        return num;
    }
};

int main()
{
    class solution test;
    int out = test.romanToInt("MCMXCIV");

    cout << out << endl;
    while(1);
    return 0;
}