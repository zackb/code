#pragma once

#include "corex/components/Component.h"

struct Interactable : public Component {
    enum class Type { Pickup, Door, None };
    Type type;
    bool hasInteracted = false;
    Interactable(Type type) : type(type) {}
};
