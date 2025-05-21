#pragma once
#include "assets/Definitions.h"

#include <memory>

struct Player {
    PlayerDefinition def;
    std::shared_ptr<SpriteSheetDefinition> spriteDef;
    AttackDefinition attack;
    Player(const PlayerDefinition& d, const std::shared_ptr<SpriteSheetDefinition>& s, const AttackDefinition& a)
        : def(d), spriteDef(s), attack(a) {}
};
