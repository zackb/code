#pragma once
#include "components/TileMap.h"
#include "entity/Entity.h"

class DebugSystem {
public:
    void update(const std::shared_ptr<Entities>& entities, std::shared_ptr<TileMap>& tileMap) const;
};