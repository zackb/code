#include <cassert>
#include <string>
#include <unordered_set>

class Solution {
public:
    int lengthOfLongestSubstring(std::string s) {
        int max = 0;
        for (int i = 0; i < s.length(); i++) {
            int current = 0;
            std::unordered_set<char> set;
            for (int j = i; j < s.length(); j++) {
                if (set.contains(s[j])) {
                    break;
                }
                set.emplace(s[j]);
                current++;
                if (current > max) {
                    max = current;
                }
            }
        }
        return max;
    }
};

// Example 1:
// Input: s = "abcabcbb"
// Output: 3
// Explanation: The answer is "abc", with the length of 3.
//
// Example 2:
// Input: s = "bbbbb"
// Output: 1
// Explanation: The answer is "b", with the length of 1.
//
// Example 3:
// Input: s = "pwwkew"
// Output: 3
// Explanation: The answer is "wke", with the length of 3.
// Notice that the answer must be a substring, "pwke" is a subsequence and not a substring.
int main() {
    Solution s;
    assert(s.lengthOfLongestSubstring("abcabcbb") == 3);
    assert(s.lengthOfLongestSubstring("bbbbb") == 1);
    assert(s.lengthOfLongestSubstring("pwwkew") == 3);
    return 0;
}
