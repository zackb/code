#pragma once

#include "components/Background.h"
#include "components/TileMap.h"

struct ParallaxLayer {
    SDL_Texture* texture;
    float speed;
};

class RenderSystem {
public:
    void render(const std::vector<std::shared_ptr<Entity>>& entities, TileMap& tileMap);

private:
    void renderParallaxBackground(Entity& background, const Transform& camera) const;
    void renderLayer(const Background& layer, const Transform& camera) const;
    void renderTileMap(TileMap& tileMap, Transform& camera);
    std::shared_ptr<Entity> findActiveCamera(const std::vector<std::shared_ptr<Entity>>& entities) const;
    std::shared_ptr<Entity> findBackground(const std::vector<std::shared_ptr<Entity>>& entities) const;
};
