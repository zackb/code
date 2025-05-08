#pragma once
#include "ecs/ECS.h"
#include <vector>

class CameraSystem {
public:
    void update(std::vector<std::shared_ptr<Entity>>& entities, const TileMap& map) const;
};