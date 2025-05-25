#include "systems/AnimationSystem.h"
#include "components/Animation.h"
#include "components/State.h"

void AnimationSystem::update(const std::shared_ptr<Entities>& entities, const int deltaTime) const {

    for (auto& entity : entities->findByComponents<AnimationComponent, State>()) {

        const auto animation = entity->getComponent<AnimationComponent>();

        // Update animation timing
        animation->update(deltaTime);

        // Update animation state based on entity state
        auto state = entity->getComponent<State>();

        switch (state->currentAction) {
        case Action::IDLE:
            animation->setState(AnimationState::IDLE);
            break;
        case Action::WALKING:
            animation->setState(AnimationState::WALKING);
            break;
        case Action::JUMPING:
            animation->setState(AnimationState::JUMPING);
            break;
        case Action::FALLING:
            animation->setState(AnimationState::FALLING);
            break;
        case Action::ATTACKING:
            animation->setState(AnimationState::ATTACKING);
            break;
        case Action::COLLECTING:
            animation->setState(AnimationState::COLLECTING);
            break;
        case Action::DYING:
            animation->setState(AnimationState::DYING);
            break;
        default:
            std::cerr << "Unknown action! " << std::endl;
            break;
        }
    }
}
