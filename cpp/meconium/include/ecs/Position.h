#pragma once

#include "Component.h"

class Position : public Component {
public:
    int x, y;
    bool onGround = true;

    Position(int initX, int initY) : x(initX), y(initY) {}
};
