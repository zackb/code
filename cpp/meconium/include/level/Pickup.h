#pragma once

#include "assets/Definitions.h"

struct Pickup {
    PickupDefinition def;
    std::shared_ptr<SpriteSheetDefinition> spriteSheet;
    Pickup(const PickupDefinition& def, const std::shared_ptr<SpriteSheetDefinition>& spriteSheet)
        : def(def), spriteSheet(spriteSheet) {}
};

using Pickups = std::vector<std::shared_ptr<Pickup>>;
