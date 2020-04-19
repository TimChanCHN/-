#include <iostream>
#include <vector>
#include <map>
#include <stack>

/*
    给定一个只包括 '('，')'，'{'，'}'，'['，']' 的字符串，判断字符串是否有效。
    方法：
        利用栈的思想，如果遇到的是左边的则入栈，遇到右边，则判断栈顶是否相等，是则pop出栈顶，继续遍历
 */

using namespace std;

class solution
{
public:
    bool isValid(string s)
    {
        stack<char> stack;

        if( s.size() % 2 != 0 )
            return false;

        for( int i = 0; i < s.size(); i++ )
        {
            if( (s[i] == '(') || (s[i] == '[') || (s[i] == '{') )
                stack.push(s[i]);
            else if( (s[i] == ')') || (s[i] == ']') || (s[i] == '}') )
            {
                if( s[i] == ')')
                {
                    if( stack.empty() || stack.top()!='(')
                    {
                        return false;
                    }
                }
                else if( s[i] == ']' )
                {
                    if( stack.empty() || stack.top() != '[' )
                    {
                        return false;
                    }
                }
                else
                {
                    if( stack.empty() || stack.top() != '{' )
                        return false;
                }
                stack.pop();
            }
            else
            {
                return false;
            }
        }

        /* 以上判断并不能解决位数未偶数且只有左边的情况，需要利用empty来排除 */
        return stack.empty();
    }
};

int main()
{
    class solution test;
    bool out = test.isValid("((");

    cout << out << endl;
    while(1);
    return 0;
}