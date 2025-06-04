#include "corex/systems/InteractionSystem.h"
#include "corex/Utils.h"
#include "corex/Vec2.h"
#include "corex/components/Bag.h"
#include "corex/components/Collider.h"
#include "corex/components/Despawn.h"
#include "corex/components/Health.h"
#include "corex/components/Interactable.h"
#include "corex/components/Pickup.h"
#include "corex/components/State.h"
#include "corex/components/Tag.h"
#include "corex/components/Transform.h"
#include "corex/components/Tween.h"

void InteractionSystem::update(Entities& entities) {

    auto player = entities.findEntityWithComponent<PlayerTag>();

    if (!player)
        return;

    for (auto& entity : entities.filtered<Interactable, Collider>()) {
        resolveInteraction(entities, *player, *entity);
    }
}

void InteractionSystem::resolveInteraction(Entities& entities, Entity& player, Entity& interactable) const {
    auto playerPos = player.getComponent<Transform>();
    auto interactablePos = interactable.getComponent<Transform>();
    if (!playerPos || !interactablePos) {
        std::cerr << "missing position for interactable\n";
        return;
    }

    auto playerCollider = player.getComponent<Collider>();
    auto pickupCollider = interactable.getComponent<Collider>();

    auto playerRect = playerCollider->getBounds(playerPos);
    auto interactableRect = pickupCollider->getBounds(interactablePos);

    if (util::aabb(playerRect, interactableRect) && !interactable.hasComponent<Despawn>()) {

        // remove the collider from the interactable so we dont keep bumping into it
        entities.removeComponent<Collider>(interactable);
        auto intComponent = interactable.getComponent<Interactable>();

        switch (intComponent->type) {
        case Interactable::Type::Pickup:
            resolvePickup(entities, player, interactable);
            break;
        case Interactable::Type::Door:
            break;
        case Interactable::Type::None:
            std::cerr << "None interactable detected\n";
            break;
        }
    }
}

void InteractionSystem::resolvePickup(Entities& entities, Entity& player, Entity& pickup) const {

    auto pickupComp = pickup.getComponent<Pickup>();

    if (!pickupComp) {
        std::cerr << "Pickup interactable is missing Pickup component\n";
        return;
    }

    auto playerHealth = player.getComponent<Health>();
    auto pickupPos = pickup.getComponent<Transform>();

    // TODO: all pickup types (bag)
    switch (pickupComp->type) {

    case Pickup::Type::HEALTH:
        playerHealth->hp = std::min(playerHealth->max, playerHealth->hp += pickupComp->amount);

        // TODO: action/animation duration
        pickup.addComponent<Tween>(Vec2{static_cast<float>(pickupPos->x), static_cast<float>(pickupPos->y)},
                                   Vec2{static_cast<float>(pickupPos->x), static_cast<float>(pickupPos->y) - 200},
                                   500.0f,
                                   EasingType::EaseOutQuad);
        pickup.getComponent<State>()->lockAction(Action::COLLECTING, 1000);
        pickup.addComponent<Despawn>(1000);
        break;

    case Pickup::Type::KEY: {
        // TODO: HACK has to be defined in the prefab
        pickup.addComponent<Tween>(Vec2{static_cast<float>(pickupPos->x), static_cast<float>(pickupPos->y)},
                                   Vec2{20.0f, 30.0f},
                                   2000.0f,
                                   EasingType::EaseOutBounce,
                                   true);

        // TODO: action/animation duration
        pickup.getComponent<State>()->lockAction(Action::COLLECTING, 2000, [&]() {
            // TODO: if the player dies mid-animation the player reference is invalid
            //       probably pass a copy of the shared_ptr instead
            if (auto bag = player.getComponent<Bag>()) {
                bag->add(pickup);
            }
            pickup.addComponent<Despawn>();
        });

        break;
    }
    default:
        std::cerr << "no handler for pickup type\n";
        break;
    }
}
