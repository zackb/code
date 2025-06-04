#pragma once

#include "corex/components/Component.h"

struct GrantHealth : public Component {
    int amount;
    GrantHealth(int amt) : amount(amt) {}
};
