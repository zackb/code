#pragma once

#include "corex/assets/Definitions.h"

#include <memory>

struct Enemy {
    EnemyDefinition def;
    std::shared_ptr<SpriteSheetDefinition> spriteSheet;
    bool hasSpawned = false;
};

using Enemies = std::vector<std::shared_ptr<Enemy>>;
