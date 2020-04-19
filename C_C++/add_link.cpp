#include <iostream>
#include <vector>
#include <map>
#include <stack>

/*
    对链表l1,l2合并成一条新链表，并且链表中的value要从小到大排序
    关键：
        l1,l2均不为空的时候才要进行比较，若一条链表空但是另外一条链表不空，则合成链表的结尾部分是非空链表的剩余部分。
 */

using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

class solution
{
public:
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    {
        ListNode* head = new ListNode(0);
        ListNode* nownode = head;

        if( l1 == NULL )
            return l2;
        if( l2 == NULL )
            return l1;

        while (l1 && l2 )
        {
            if( l1->val <= l2->val )
            {
                nownode->next = l1;
                l1 = l1->next;
            }
            else
            {
                nownode->next = l2;
                l2 = l2->next;
            }
            nownode = nownode->next;
        }
        
        nownode->next = l1?l1:l2;

        return head->next;
    }
};

int main()
{
    class solution test;
    bool out = test.mergeTwoLists();

    cout << out << endl;
    while(1);
    return 0;
}