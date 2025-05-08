#pragma once

#include "Animation.h"
#include "Camera.h"
#include "Context.h"
#include "Size.h"
#include "TileMap.h"
#include <SDL.h>

class RenderSystem {
public:
    void render(const std::vector<std::shared_ptr<Entity>>& entities, TileMap& tileMap);

private:
    void renderTileMap(TileMap& tileMap, Camera& camera);
};
