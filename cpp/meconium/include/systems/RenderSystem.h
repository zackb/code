#pragma once

#include "TileMap.h"

class RenderSystem {
public:
    void render(const std::vector<std::shared_ptr<Entity>>& entities, TileMap& tileMap);

private:
    void renderTileMap(TileMap& tileMap, Transform& camera);
    std::shared_ptr<Entity> findActiveCamera(const std::vector<std::shared_ptr<Entity>>& entities) const;
};
