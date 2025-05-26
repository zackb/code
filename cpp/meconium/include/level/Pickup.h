#pragma once

#include "assets/Definitions.h"

// TODO: Move to component
struct Pickup {
    enum class Type { HEALTH, KEY };
    Type type;
    PickupDefinition def;
    std::shared_ptr<SpriteSheetDefinition> spriteSheet;

    Pickup(const PickupDefinition& def, Type type, const std::shared_ptr<SpriteSheetDefinition>& spriteSheet)
        : def(def), type(type), spriteSheet(spriteSheet) {}
};

using Pickups = std::vector<std::shared_ptr<Pickup>>;
