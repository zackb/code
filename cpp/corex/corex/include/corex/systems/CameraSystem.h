#pragma once

#include "corex/components/TileMap.h"
#include "corex/entity/Entity.h"

class CameraSystem {

public:
    void update(Entities& entities, const TileMap& map) const;
};
