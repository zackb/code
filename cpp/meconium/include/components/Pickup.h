#pragma once

#include "components/Component.h"

struct Pickup : public Component {

    enum class Type { HEALTH, KEY };
    Type type;
    int amount;

    Pickup(Type type, int amount) : type(type), amount(amount) {}
};
