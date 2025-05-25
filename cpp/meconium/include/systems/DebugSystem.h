#pragma once

#include "components/TileMap.h"
#include "entity/Entity.h"

class DebugSystem {

public:
    void update(Entities& entities, TileMap& tileMap) const;
};
