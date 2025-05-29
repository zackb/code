#pragma once

#include "components/TileMap.h"
#include "entity/Entity.h"
#include <SDL_ttf.h>

class DebugSystem {

public:
    DebugSystem();
    ~DebugSystem();
    void update(Entities& entities, TileMap& tileMap) const;

private:
    TTF_Font* font;
    void renderTextUpperRight(const char* text) const;
};
