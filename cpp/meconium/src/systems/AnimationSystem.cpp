#include "ECS.h"
#include "components/State.h"

void AnimationSystem::update(const std::shared_ptr<Entities>& entities, const int deltaTime) const {
    for (auto& entity : *entities) {
        const auto animation = entity->getComponent<AnimationComponent>();
        if (!animation)
            continue;

        // Update animation timing
        animation->update(deltaTime);

        // Update animation state based on entity state
        auto state = entity->getComponent<State>();
        if (!state)
            continue;

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
        default:
            std::cerr << "Unknown action! " << std::endl;
            break;
        }
    }
}
