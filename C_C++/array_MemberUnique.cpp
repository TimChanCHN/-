#include <iostream>
#include <vector>
#include <map>
#include <stack>

/*
    给定一个排序数组，你需要在 原地 删除重复出现的元素，使得每个元素只出现一次，返回移除后数组的新长度。
    关键：
        原数组本来就是排序的，当发现第i个元素与i+1相等时，把i元素删除并且数组长度减1即可
 */
using namespace std;

class solution
{
public:
    int removeDuplicates(vector<int>& nums)
    {
        int len = nums.size();
        for( int i = 0; i < len-1; i++ )
        {
            if( nums[i] == nums[i+1] )
            {
                nums.erase(nums.begin()+i);
                i--;
                len--;
            }
        }
        return len;
    }
};

int main()
{
    class solution test;
    vector<int> a{0,0,1,1,1,2,2,3,3,4};
    int out = test.removeDuplicates(a);

    cout << out << endl;
    while(1);
    return 0;
}