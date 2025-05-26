#pragma once

#include "components/Animation.h"
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
    std::shared_ptr<AnimationComponent> animation;

    Item(Pickup::Type type,
         int amount,
         const std::shared_ptr<Sprite>& sprite,
         const std::shared_ptr<AnimationComponent>& anim)
        : type(type), amount(amount), sprite(sprite), animation(anim) {}
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
        auto animation = entity.getComponent<AnimationComponent>();

        items.emplace_back(pickup->type, pickup->amount, sprite, animation);
    }
};
