#pragma once

#include "corex/components/AddToBag.h"
#include "corex/components/Animation.h"
#include "corex/components/Component.h"
#include "corex/components/Sprite.h"
#include "corex/entity/Entity.h"
#include <algorithm>
#include <memory>
#include <vector>

struct Item {
    std::string itemId;
    std::shared_ptr<Sprite> sprite;
    std::shared_ptr<AnimationComponent> animation;

    Item(std::string itemId, const std::shared_ptr<Sprite>& sprite, const std::shared_ptr<AnimationComponent>& anim)
        : itemId(itemId), sprite(sprite), animation(anim) {}
};

struct Bag : public Component {

    std::vector<Item> items;

    void add(Entity& entity) {
        auto sprite = entity.getComponent<Sprite>();
        if (!sprite) {
            std::cerr << "tried to collect an item without a sprite\n";
            return;
        }

        auto addToBag = entity.getComponent<AddToBag>();
        if (!addToBag) {
            std::cerr << "tried to collect an item without an AddToBag\n";
            return;
        }
        auto animation = entity.getComponent<AnimationComponent>();

        items.emplace_back(addToBag->itemId, sprite, animation);
    }

    bool contains(const std::string& itemId) {
        return std::any_of(items.begin(), items.end(), [&](const auto& item) { return item.itemId == itemId; });

        /*
        for (auto item : items) {
            if (item.itemId == itemId) {
                return true;
            }
        }
        return false;
        */
    }
};
