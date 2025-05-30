#pragma once

#include "Vec2.h"
#include "components/Component.h"

struct Tween : public Component {
    Vec2 startPos;
    Vec2 endPos;
    float duration;
    float elapsed;
    bool finished;
    Tween(const Vec2& startPos, const Vec2& endPos, float duration)
        : startPos(startPos), endPos(endPos), duration(duration), elapsed(0.0), finished(false) {}
};
