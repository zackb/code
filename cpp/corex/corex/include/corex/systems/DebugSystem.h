#pragma once

#include "corex/components/TileMap.h"
#include "corex/entity/Entity.h"
#include <SDL_ttf.h>

class DebugSystem {

public:
    DebugSystem();
    ~DebugSystem();
    void update(Entities& entities, TileMap& tileMap);

private:
    TTF_Font* font = nullptr;
    void renderTextUpperRight(const char* text) const;
};
