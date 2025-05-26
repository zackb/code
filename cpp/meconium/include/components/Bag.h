#pragma once

#include "components/Component.h"
#include "components/Pickup.h"
#include "components/Sprite.h"
#include "entity/Entity.h"
#include <memory>
#include <vector>

struct Item {
    Pickup::Type type;
    int amount;
    std::shared_ptr<Sprite> sprite;

    Item(Pickup::Type type, int amount, const std::shared_ptr<Sprite>& sprite)
        : type(type), amount(amount), sprite(sprite) {}
};

struct Bag : public Component {

    std::vector<Item> items;

    void add(Entity& entity) {
        auto sprite = entity.getComponent<Sprite>();
        if (!sprite) {
            std::cerr << "tried to collect an item without a sprite\n";
            return;
        }

        auto pickup = entity.getComponent<Pickup>();
        if (!pickup) {
            std::cerr << "tried to collect an item without a pickup\n";
            return;
        }

        items.emplace_back(pickup->type, pickup->amount, sprite);
    }
};
