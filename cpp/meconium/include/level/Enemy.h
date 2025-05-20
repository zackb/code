#pragma once

#include "assets/Definitions.h"

#include <memory>

struct Enemy {
    EnemyDefinition def;
    std::shared_ptr<SpriteSheetDefinition> spriteSheet;
    bool hasSpawned = false;
};
