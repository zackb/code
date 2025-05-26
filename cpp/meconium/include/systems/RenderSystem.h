#pragma once

#include "components/Background.h"
#include "components/TileMap.h"
#include "components/Transform.h"
#include "entity/Entity.h"

struct ParallaxLayer {
    SDL_Texture* texture;
    float speed;
};

class RenderSystem {
public:
    void render(Entities& entities, TileMap& tileMap);

private:
    void renderParallaxBackground(Entity& background, const Transform& camera) const;
    void renderLayer(const Background& layer, const Transform& camera) const;
    void renderTileMap(const TileMap& tileMap, const Transform& camera);
};
