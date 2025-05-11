#pragma once
#include "entity/Entity.h"

class DebugSystem {
public:
    void update(const std::shared_ptr<Entities>& entities) const;
};