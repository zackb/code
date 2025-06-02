#pragma once

#include "corex/components/Component.h"

struct Health : public Component {
    int hp;
    int max = 100;
    Health(int max) : hp(max), max(max) {}
};
