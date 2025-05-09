#pragma once

#include "TileMap.h"

struct ParallaxLayer {
    SDL_Texture* texture;
    float speed;
};

class RenderSystem {
public:
    void render(const std::vector<std::shared_ptr<Entity>>& entities, TileMap& tileMap);

private:
    void renderParallaxBackground(Transform& camera);
    void renderLayer(const ParallaxLayer& layer, const Transform& camera);
    void renderTileMap(TileMap& tileMap, Transform& camera);
    std::shared_ptr<Entity> findActiveCamera(const std::vector<std::shared_ptr<Entity>>& entities) const;
};
