#include <iostream>

struct ListNode {
    int val;
    ListNode* next;

    ListNode() : val(0), next(nullptr) {}
    ListNode(int val) : val(val), next(nullptr) {}
    ListNode(int val, ListNode* next) : val(val), next(next) {}

    ~ListNode() {
        if (next)
            delete next;
    }
};

class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        auto result = new ListNode();

        auto c1 = l1;
        auto c2 = l2;
        auto r = result;
        int remainder = 0;
        do {
            int sum = 0 + (c1 ? c1->val : 0) + (c2 ? c2->val : 0) + remainder;
            r->val = sum % 10;
            remainder = sum / 10;
            if (c1)
                c1 = c1->next;
            if (c2)
                c2 = c2->next;
            if (c1 || c2) {
                r->next = new ListNode();
                r = r->next;
            }
        } while (c1 || c2);

        if (remainder > 0) {
            r->next = new ListNode(remainder);
        }
        return result;
    }
};

void print(ListNode* list) {

    do {
        std::cout << list->val << std::endl;
    } while ((list = list->next));
    std::cout << std::endl;
}

// Input: l1 = [2,4,3], l2 = [5,6,4]
// Output: [7,0,8]
// Explanation: 342 + 465 = 807.
int main() {
    auto l1 = new ListNode(2, new ListNode(4, new ListNode(3)));
    auto l2 = new ListNode(5, new ListNode(6, new ListNode(4)));
    Solution s;
    auto res = s.addTwoNumbers(l1, l2);
    print(res);

    delete l1;
    delete l2;
    delete res;

    l1 = new ListNode(0);
    l2 = new ListNode(0);
    res = s.addTwoNumbers(l1, l2);
    print(res);

    delete res;
    delete l1;
    delete l2;

    l1 = new ListNode(
        9, new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9)))))));
    l2 = new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9))));
    res = s.addTwoNumbers(l1, l2);
    print(res);

    delete l1;
    delete l2;
    delete res;

    return 0;
}
