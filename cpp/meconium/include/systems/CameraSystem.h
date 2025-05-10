#pragma once

#include "components/TileMap.h"

class CameraSystem {
public:
    void update(const std::shared_ptr<Entities>& entities, const TileMap& map) const;
};