#pragma once

#include <SDL_rect.h>
#include <algorithm>
#include <cctype>
#include <string>

namespace util {
    inline std::string tolower(const std::string& input) {
        std::string result = input;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    inline bool aabb(SDL_Rect& a, SDL_Rect& b) {
        return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
    }
}; // namespace util
