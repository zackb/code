#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace util {
    inline std::string tolower(const std::string& input) {
        std::string result = input;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
        return result;
    }
}; // namespace util
