#pragma once

#include <SDL.h>
#include "TileMap.h"
#include "Context.h"
#include "Camera.h"
#include "Size.h"
#include "Animation.h"

class RenderSystem {
public:
    void render(const std::vector<std::shared_ptr<Entity> > &entities, TileMap &tileMap);

private:
    void renderTileMap(TileMap &tileMap, Camera &camera);
};
