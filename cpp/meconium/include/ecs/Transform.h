#pragma once

#include "Component.h"

class Transform final : public Component {
public:
    // TODO: FLOOOATS
    int x, y;                         // Position in world coordinates
    float scaleX = 1.0, scaleY = 1.0; // Scaling factors for size
    bool onGround = true;

    Transform(int initX, int initY) : x(initX), y(initY) {}
    Transform(int initX, int initY, float scale) : x(initX), y(initY), scaleX(scale), scaleY(scale) {}
    Transform(int initX, int initY, float scaleX, float scaleY) : x(initX), y(initY), scaleX(scaleX), scaleY(scaleY) {}
};
