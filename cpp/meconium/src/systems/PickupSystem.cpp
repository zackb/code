#include "systems/PickupSystem.h"
#include "Utils.h"
#include "Vec2.h"
#include "components/Bag.h"
#include "components/Collider.h"
#include "components/Despawn.h"
#include "components/Health.h"
#include "components/Pickup.h"
#include "components/State.h"
#include "components/Tag.h"
#include "components/Transform.h"
#include "components/Tween.h"

void PickupSystem::update(Entities& entities) {

    auto player = entities.findEntityWithComponent<PlayerTag>();

    if (!player)
        return;

    for (auto& entity : entities.filtered<PickupTag>()) {
        resolvePlayerPickupCollisions(entities, *player, *entity);
    }
}

void PickupSystem::resolvePlayerPickupCollisions(Entities& entities, Entity& player, Entity& pickup) const {
    auto playerPos = player.getComponent<Transform>();
    auto pickupPos = pickup.getComponent<Transform>();
    if (!playerPos || !pickupPos) {
        std::cerr << "missing position for pickup\n";
        return;
    }

    auto playerCollider = player.getComponent<Collider>();
    auto pickupCollider = pickup.getComponent<Collider>();

    auto playerRect = playerCollider->getBounds(playerPos);
    auto pickupRect = pickupCollider->getBounds(pickupPos);

    if (util::aabb(playerRect, pickupRect) && !pickup.hasComponent<Despawn>()) {
        auto playerHealth = player.getComponent<Health>();

        if (auto pickupComp = pickup.getComponent<Pickup>()) {
            // TODO: all pickup types (bag)
            switch (pickupComp->type) {
            case Pickup::Type::HEALTH:
                playerHealth->hp = std::min(playerHealth->max, playerHealth->hp += pickupComp->amount);

                // TODO: action/animation duration
                pickup.addComponent<Tween>(
                    Vec2{static_cast<float>(pickupPos->x), static_cast<float>(pickupPos->y)},
                    Vec2{static_cast<float>(pickupPos->x), static_cast<float>(pickupPos->y) - 200},
                    500.0f,
                    EasingType::EaseOutQuad);
                pickup.getComponent<State>()->lockAction(Action::COLLECTING, 1000);
                pickup.addComponent<Despawn>(1000);
                break;
            case Pickup::Type::KEY: {
                if (auto bag = player.getComponent<Bag>()) {
                    bag->add(pickup);
                }
                // TODO: HACK has to be defined in the prefab
                pickup.addComponent<Tween>(Vec2{static_cast<float>(pickupPos->x), static_cast<float>(pickupPos->y)},
                                           Vec2{20.0f, 30.0f},
                                           2000.0f,
                                           EasingType::EaseOutBounce,
                                           true);

                // TODO: action/animation duration
                pickup.getComponent<State>()->lockAction(Action::COLLECTING, 2000);
                pickup.addComponent<Despawn>(2000);

                break;
            }
            default:
                std::cerr << "no handler for pickup type\n";
                break;
            }
        } else {
            std::cerr << "pickup component missing from pickup entity\n";
        }
    }
}
