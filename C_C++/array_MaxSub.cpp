#include <iostream>
#include <vector>
#include <map>
#include <stack>

/*
    给定一个整数数组 nums ，找到一个具有最大和的连续子数组（子数组最少包含一个元素），返回其最大和。
    解法：
        1. 暴力解法：穷举出每种连续排列，算出最大值
        2. 动态规划：记录每个第i位结束的字串最大值，所有max的最大值即为全部的最大值。当i-1位的max<0是，最大值就是nums[i]，否则就是max[i-1]+nums[i]
 */
using namespace std;

class solution
{
public:
    /* 穷举法 */
    int maxSubArray(vector<int>& nums)
    {
        int len = nums.size();
        int max1 = INT_MIN;

        for( int i = 0; i < len; i++ )
        {
            int sum = 0;
            for( int j = i; j < len; j++ )
            {
                sum += nums[j];
                max1 = max(max1, sum);
            }
        }
        return max1;
    }

    /* 动态规划 */
    int maxSubArray1(vector<int>& nums)
    {
        int len = nums.size();
        int dp[len];                //记录以第i位结束的字串最大值

        dp[0] = nums[0];
        for( int i = 1; i < len; i++ )
        {
            if( dp[i-1] >= 0 )
                dp[i] = dp[i-1] + nums[i];
            else
            {
                dp[i] = nums[i];
            }
        }

        int max1 = dp[0];
        for( int i = 1; i < len; i++ )
        {
            max1 = max(max1,dp[i]); 
        }
        return max1;
    }
};

int main()
{
    vector<int> num{-2,1,-3,4,-1,2,1,-5,4};
    class solution test;
    int out = test.maxSubArray1(num);

    cout << out << endl;
    while(1);
    return 0;
}