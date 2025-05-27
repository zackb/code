#include <iostream>
#include <vector>

class Solution {

public:
    std::vector<int> twoSum(std::vector<int>& nums, int target) {
        for (int i = 0; i < nums.size(); i++) {
            for (int j = i + 1; j < nums.size(); j++) {
                if (nums[i] + nums[j] == target)
                    return {i, j};
            }
        }
        return {};
    }
};

int main(int argc, const char** argv) {
    Solution s;
    std::vector<int> q{1, 2, 6, 7, 10};
    auto res = s.twoSum(q, 9);
    if (res.size() == 2)
        std::cout << "Result: " << res[0] << " : " << res[1] << std::endl;
    else
        std::cerr << "no result" << std::endl;
    return 0;
}
