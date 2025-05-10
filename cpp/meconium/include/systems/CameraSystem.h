#pragma once

#include "../components/TileMap.h"
#include "ECS.h"
#include <vector>

class CameraSystem {
public:
    void update(std::vector<std::shared_ptr<Entity>>& entities, const TileMap& map) const;
};