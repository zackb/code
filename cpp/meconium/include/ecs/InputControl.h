#pragma once

#include "Component.h"

class InputControl : public Component
{
public:
    bool up, down, left, right;
    InputControl() : up(false), down(false), left(false), right(false) {}
};
