#pragma once
#include "assets/Definitions.h"

#include <memory>

struct Player {
    PlayerDefinition def;
    std::shared_ptr<SpriteSheetDefinition> spriteDef;
    AttackDefinition attack;
    std::optional<Rect> hitbox;

    Player(const PlayerDefinition& d,
           const std::shared_ptr<SpriteSheetDefinition>& s,
           const AttackDefinition& a,
           const std::optional<Rect>& h)
        : def(d), spriteDef(s), attack(a), hitbox(h) {}

    Player(const PlayerDefinition& d, const std::shared_ptr<SpriteSheetDefinition>& s, const AttackDefinition& a)
        : def(d), spriteDef(s), attack(a) {}
};
