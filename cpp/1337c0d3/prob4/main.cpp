#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

class Solution {
public:
    std::string longestPalindrome(std::string s) {
        if (s.empty())
            return "";

        int start = 0, maxLen = 1;

        auto expand = [&](int left, int right) {
            while (left >= 0 && right < s.size() && s[left] == s[right]) {
                int len = right - left + 1;
                if (len > maxLen) {
                    start = left;
                    maxLen = len;
                }
                --left;
                ++right;
            }
        };

        for (int i = 0; i < s.size(); i++) {
            expand(i, i);     // odd-length
            expand(i, i + 1); // even-length
        }

        return s.substr(start, maxLen);
    }
    std::string longestPalindromeOn3(std::string s) {

        auto result = std::string(1, s[0]);

        for (int i = 0; i < s.length(); i++) {
            for (int j = i + 1; j <= s.length(); j++) {
                std::string sub = s.substr(i, j - i);
                std::string rev(sub);
                std::reverse(rev.begin(), rev.end());
                if (sub == rev) {
                    if (sub.length() > result.length()) {
                        result = sub;
                    }
                }
            }
        }
        return result;
    }
};

// Input: s = "babad"
// Output: "bab"
// Explanation: "aba" is also a valid answer.
int main() {
    Solution s;
    assert(s.longestPalindrome("babad") == "bab");
    assert(s.longestPalindrome("ab") == "a");
    assert(s.longestPalindrome("a") == "a");
    assert(s.longestPalindrome("bb") == "bb");
    assert(s.longestPalindrome(
               "nmngaowrbsssvihklwmuqshcddwlxrywrlwtennwfvrevgvhsvgeccfulmuvrcksdmgeqrblnlwoepefhcwhmgyvgcoyyygrmttyfyc"
               "xwbqktpurlcfhzlakhmrddsydgygganpmaglaxyhfwjusukzcnakznygqplngnkhcowavxoiwrfycxwdkxqfcjqwyqutcpyedbnuoge"
               "dwobsktgioqdczxhikjrbkmqspnxcpngfdwdaboscqbkwforihzqdcppxjksiujfvlpdjryewaxgmdgigvxdlstxwngtbdrrkfudjin"
               "zyxbdmkautclvvyguekuzwwetmsxittgtxbnvvrgasvnlogdiepltweaehubwelznidltzlbzdsrxmhjpkmylnwkdsxnpkplkdzywio"
               "luaqguowtbaoqzqgjfewphqcvlnwlojbxgomvxxkhwwykawegxubjiobizicuxzeafgautefsurgjlbhcfevqzsbhwxycrcaibdsglu"
               "czcuewzqupakbzmcvzsfodbmgtugnihyhqkvyeboqhqldifbxuaxqzxtyejoswikbzpsvzkxcndgeyvfnyrfbkhlalzpqjueibnodam"
               "gpnxlkvwvliouvejcpnakllfxepldfmdzszagkyhdgqqbkb") == "uczcu");
    return 0;
}
