#pragma once

#include "Vec2.h"
#include "components/Component.h"

struct Tween : public Component {
    Vec2 startPos;
    Vec2 endPos;
    float duration;
    float elapsed;
    bool finished;
};
