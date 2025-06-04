#pragma once

#include "corex/components/Component.h"

struct Interactable : public Component {
    enum class Type { Pickup, Door, None };
    Type type;
    Interactable(Type type) : type(type) {}
};
