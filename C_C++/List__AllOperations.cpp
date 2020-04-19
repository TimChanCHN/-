#include <iostream>
#include <vector>
#include <map>
#include <stack>

/*
    给定一个有序链表，删除所有重复的元素，使得每个元素只出现一次：deleteDuplicates
    给定一个无序链表，删除所有重复的元素，使得每个元素只出现一次：deleteDuplicates1
    给定一个无序链表，删除所有重复的元素，使得每个元素只出现一次，并对其从小到大排序，：deleteDuplicates2
    给定一个无序链表，删除所有重复的元素，使得每个元素只出现一次，同时对一个值进行插值，：InsertOneElement
    给定一个无序链表，删除所有重复的元素，使得每个元素只出现一次，同时对一个值进行删除，：RemoveOneElement
 */
using namespace std;
/* 该结构体最好添加number到链表中 */
struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

ListNode* CreateList()
{
    int num, value;
    ListNode* head = new ListNode(0);
    ListNode* begin = head;

    cout << "pls input the numbers of the nodes:";
    cin >> num;

    for( int i = 0; i < num; i++ )
    {
        cout << "pls input the " << i+1 << " node: ";
        cin >> value;
        ListNode* node = new ListNode(value);
        head->next = node;
        head = node;
    }

    return begin->next;
}

class Solution {
public:
    /* 1. 对有序链表进行去重 */
    ListNode* deleteDuplicates(ListNode* head) {
        ListNode* current = head;
        // 最后一个不用参与比较，因为判断的时候可以覆盖到最后一个
        while( current->next != NULL )
        {
            if( current->val == current->next->val )
            {
                ListNode* tmp = current->next;
                current->next = tmp->next;
                delete tmp;
            }
            else
            {
                current = current->next;
            }
        }
        return head;
    }

    /* 2. 对有序链表进行去重 */
    ListNode* deleteDuplicates1(ListNode* head)
    {
        ListNode* p1 = head;
        
        while( p1 != NULL )
        {
            ListNode* p2 = p1;
            while( p2->next != NULL)
            {
                if( p1->val == p2->next->val )
                {
                    ListNode* tmp = p2->next;
                    p2->next = tmp->next;
                    delete tmp;
                }
                else
                {
                    p2 = p2->next;
                }
            }
            p1 = p1->next;
        }
        return head;
    }

    /* 3. 对不重合链表进行排序 */
    ListNode* deleteDuplicates2(ListNode* head)
    {
        ListNode *p1, *p2, *p3;
        p1 = head;

        for( p1 = head; p1 != NULL; p1 = p1->next )
        {
            p3 = p1;
            for( p2 = p1->next; p2 != NULL; p2 = p2->next )
            {
                if( p3->val > p2->val )
                    p3 = p2;
            }
            if( p1 != p3 )
            {
                int tmp = p1->val;
                p1->val = p3->val;
                p3->val = tmp;
            }
        }
        return head;
    }

    /* 4. 增加元素到升序链表中 */
    ListNode* InsertOneElement(ListNode* head, int value)
    {
        ListNode* node = head;

        /* 1. 如果tmp在表头 */
        if( value < node->val )
        {
            ListNode* tmp = new ListNode(value);
            tmp->next = node;
            head = tmp;
        }
        else if( value == node->val )
        {
            cout << "value " << value << " is already in the list." << endl;
        }
        else
        {
            // node = node->next;
            while( node )
            {
                if( node->next != NULL )
                {
                    if( value < node->next->val )
                    {
                        ListNode* tmp = new ListNode(value);
                        tmp->next = node->next;
                        node->next = tmp;
                        break;
                    }
                    else if( value == node->next->val )
                    {
                        cout << "value " << value << " is already in the list." << endl;
                        break;
                    }
                }
                else
                {
                    if( value > node->val )
                    {
                        ListNode* tmp = new ListNode(value);
                        node->next = tmp;
                        break;
                    }
                    if( value == node->val )
                    {
                        cout << "value " << value << " is already in the list." << endl;
                        break;
                    }
                }

                node = node->next;
            }
        }
        return head;        
    }

    /* 5. 在升序链表中删除元素 */
    ListNode* RemoveOneElement(ListNode* head, int value)
    {
        ListNode* p = head;
        bool valueisexit = false;

        /* 1. 删除表头 */
        while( (p != NULL) && (value == p->val) )
        {
            ListNode* tmp = p;
            p = p->next;
            delete tmp;
            valueisexit = true;
        }
        if( p != NULL )
        {
            ListNode *p1, *p2;
            p1 = p;
            p2 = p->next;
            while( p2 != NULL )
            {
                if( p2->val == value )
                {
                    p1->next = p2->next;
                    delete p2;
                    p2 = p1->next;
                    valueisexit = true;
                }
                else
                {
                    p2 = p2->next;
                    p1 = p1->next;
                }
            }
        }
        if( !valueisexit )
        {
            cout << "value: " << value << " is not in the list." << endl;
        }
        return p;
    }
};

int main()
{
    ListNode* list = CreateList();
    ListNode* tmp;
    int data, data1;
    class Solution test;

    cout << "enter the data to add into the list:";
    cin >> data;
    cout << "enter the data to be remove in the list:";
    cin >> data1;

    /* print the list */
    tmp = list;
    cout << endl << "print the list." << endl;
    while( tmp != NULL )
    {
        cout << tmp->val << " ";
        tmp = tmp->next;
    } 
    cout << endl;

    ListNode* res = test.deleteDuplicates1(list);
    tmp = res;
    cout << "print case 1:" << endl;
    while ( tmp != NULL )
    {
        cout << tmp->val << endl;
        tmp = tmp->next;
    }

    ListNode* res1 = test.deleteDuplicates2(res);
    cout << "print case 2:" << endl;
    tmp = res1;
    while ( tmp != NULL )
    {
        cout << tmp->val << endl;
        tmp = tmp->next;
    }

    ListNode* res2 = test.InsertOneElement(res1, data);
    cout << "print case 3:" << endl;
    tmp = res2;
    while ( tmp != NULL )
    {
        cout << tmp->val << endl;
        tmp = tmp->next;
    }

    ListNode* res3 = test.RemoveOneElement(res2, data1);
    cout << "print case 4:" << endl;
    tmp = res3;
    while( tmp != NULL )
    {
        cout << tmp->val << endl;
        tmp = tmp->next;
    }
    
    /* 清空链表 */
    while( res3 != NULL )
    {
        ListNode* tmp = res3;
        res3 = res3->next;
        delete tmp;
    }
    cout << "WORK DONE!!!" << endl;
    while(1);
    return 0;
}