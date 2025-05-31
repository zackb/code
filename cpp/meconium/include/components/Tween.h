#pragma once

#include "Vec2.h"
#include "components/Component.h"
#include <unordered_map>

using Easing = float (*)(float);
using Easing = float (*)(float);

enum class EasingType { None, Linear, EaseInQuad, EaseOutQuad, EaseInOutQuad, EaseOutBounce };

namespace EasingFunctions {

    inline float linear(float t) { return t; }

    inline float easeInQuad(float t) { return t * t; }

    inline float easeOutQuad(float t) { return t * (2 - t); }

    inline float easeInOutQuad(float t) { return (t < 0.5f) ? (2 * t * t) : (-1 + (4 - 2 * t) * t); }

    inline float easeOutBounce(float t) {
        if (t < 1 / 2.75f) {
            return 7.5625f * t * t;
        } else if (t < 2 / 2.75f) {
            t -= 1.5f / 2.75f;
            return 7.5625f * t * t + 0.75f;
        } else if (t < 2.5f / 2.75f) {
            t -= 2.25f / 2.75f;
            return 7.5625f * t * t + 0.9375f;
        } else {
            t -= 2.625f / 2.75f;
            return 7.5625f * t * t + 0.984375f;
        }
    }

    static const std::unordered_map<EasingType, Easing> Map = {
        {EasingType::Linear, linear},
        {EasingType::EaseInQuad, easeInQuad},
        {EasingType::EaseOutQuad, easeOutQuad},
        {EasingType::EaseInOutQuad, easeInOutQuad},
        {EasingType::EaseOutBounce, easeOutBounce},
    };
} // namespace EasingFunctions

struct Tween : public Component {
    Vec2 startPos;
    Vec2 endPos;
    float duration;
    float elapsed = 0.0;
    bool finished = false;
    Easing easing = nullptr;
    bool relativeEndPos = false; // relative to the camera or absolute pos

    Tween(const Vec2& startPos,
          const Vec2& endPos,
          float duration,
          EasingType ease = EasingType::None,
          bool relativeEndPos = false)
        : startPos(startPos), endPos(endPos), duration(duration), relativeEndPos(relativeEndPos) {

        if (ease != EasingType::None) {
            easing = EasingFunctions::Map.at(ease);
        }
    }
};
