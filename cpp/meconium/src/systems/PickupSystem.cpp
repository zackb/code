#include "systems/PickupSystem.h"
#include "Utils.h"
#include "components/Collider.h"
#include "components/Despawn.h"
#include "components/Health.h"
#include "components/State.h"
#include "components/Tag.h"
#include "components/Transform.h"

void PickupSystem::update(Entities& entities) {

    auto player = entities.findEntityWithComponent<PlayerTag>();

    if (!player)
        return;

    for (auto& entity : entities.filtered<PickupTag>()) {
        resolvePlayerPickupCollisions(*player, *entity);
    }
}

void PickupSystem::resolvePlayerPickupCollisions(Entity& player, Entity& pickup) const {
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
        auto pickupHealth = pickup.getComponent<Health>();
        if (pickupHealth) {
            playerHealth->hp = std::min(playerHealth->max, playerHealth->hp += pickupHealth->hp);
        }

        // TODO: action/animation duration
        pickup.getComponent<State>()->lockAction(Action::COLLECTING, 500);
        pickup.addComponent<Despawn>(500);
    }
}
