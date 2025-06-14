#include "corex/systems/InteractionSystem.h"

#include "corex/Utils.h"
#include "corex/Vec2.h"
#include "corex/components/Bag.h"
#include "corex/components/Collider.h"
#include "corex/components/Despawn.h"
#include "corex/components/Health.h"
#include "corex/components/Interactable.h"
#include "corex/components/LevelComponent.h"
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
    auto interactableCollider = interactable.getComponent<Collider>();

    auto playerRect = playerCollider->getBounds(playerPos);
    auto interactableRect = interactableCollider->getBounds(interactablePos);

    // check if the player collided with the interactable
    if (util::aabb(playerRect, interactableRect) && !interactable.hasComponent<Despawn>()) {

        auto intComponent = interactable.getComponent<Interactable>();

        switch (intComponent->type) {
        case Interactable::Type::Pickup:
            // remove the collider from the interactable so we dont keep bumping into it
            entities.removeComponent<Collider>(interactable);
            resolvePickup(entities, player, interactable);
            break;
        case Interactable::Type::Door:
            resolveDoor(entities, player, interactable);
            break;
        case Interactable::Type::None:
            std::cerr << "None interactable detected\n";
            break;
        }
    }
}

void InteractionSystem::resolveDoor(Entities& entities, Entity& player, Entity& door) const {
    auto openDoor = door.getComponent<OpenDoor>();
    if (!openDoor) {
        std::cerr << "interactable of type door has no OpenDoor component\n";
        return;
    }
    auto interactable = door.getComponent<Interactable>();

    if (interactable->hasInteracted) {
        // so far doors only have one interaction
        return;
    }
    auto state = door.getComponent<State>();
    auto bag = player.getComponent<Bag>();
    if (!bag || !state) {
        std::cerr << "interactable missing required door components\n";
        return;
    }

    if (openDoor->keyId == "any" || bag->contains(openDoor->keyId)) {
        interactable->hasInteracted = true;
        state->lockAction(Action::OPENING, 4000);
        entities.addComponent<DoorOpened>(player);
        entities.addComponent<GoalReached>(player);
        auto playerState = player.getComponent<State>();
        playerState->lockAction(Action::OPENING, 5000, [&]() { player.addComponent<LevelComplete>(); });
    } else {
        player.addComponent<MissingKey>();
        state->lockAction(Action::IDLE, 4000, [&]() { entities.removeComponent<MissingKey>(player); });
    }
}

void InteractionSystem::resolvePickup(Entities& entities, Entity& player, Entity& pickup) const {

    auto pickupPos = pickup.getComponent<Transform>();

    if (auto grant = pickup.getComponent<GrantHealth>()) {
        auto playerHealth = player.getComponent<Health>();
        playerHealth->hp = std::min(playerHealth->max, playerHealth->hp += grant->amount);

        // TODO: action/animation duration
        pickup.addComponent<Tween>(Vec2{static_cast<float>(pickupPos->x), static_cast<float>(pickupPos->y)},
                                   Vec2{static_cast<float>(pickupPos->x), static_cast<float>(pickupPos->y) - 200},
                                   500.0f,
                                   EasingType::EaseOutQuad);
        pickup.getComponent<State>()->lockAction(Action::COLLECTING, 1000);
        pickup.addComponent<Despawn>(1000);
    } else if (auto addToBag = pickup.getComponent<AddToBag>()) {

        // TODO: HACK has to be defined in the prefab
        pickup.addComponent<Tween>(Vec2{static_cast<float>(pickupPos->x), static_cast<float>(pickupPos->y)},
                                   Vec2{20.0f, 30.0f},
                                   2000.0f,
                                   EasingType::EaseOutBounce,
                                   true);

        // TODO: animation duration
        pickup.getComponent<State>()->lockAction(Action::COLLECTING, 2000, [&]() {
            // TODO: if the player dies mid-animation the player reference is invalid
            //       probably pass a copy of the shared_ptr instead
            if (auto bag = player.getComponent<Bag>()) {
                bag->add(pickup);
            }
            pickup.addComponent<Despawn>();
        });
    } else {
        std::cerr << "no handler for pickup type\n";
    }
}
