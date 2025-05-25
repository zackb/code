#pragma once

#include "components/TileMap.h"
#include "entity/Entity.h"

class CameraSystem {

public:
    void update(Entities& entities, const TileMap& map) const;
};
