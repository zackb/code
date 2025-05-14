#include "ECS.h"
#include "components/InputControl.h"

void AnimationSystem::update(const std::shared_ptr<Entities>& entities, const int deltaTime) const {

    for (auto& entity : *entities) {
        const auto animation = entity->getComponent<AnimationComponent>();
        if (!animation)
            continue;

        // Update animation timing
        animation->update(deltaTime);

        // Update animation state based on entity state
        auto position = entity->getComponent<Transform>();
        auto velocity = entity->getComponent<Velocity>();

        if (!position || !velocity)
            continue;

        // Determine animation state
        if (!position->onGround) {
            // In the air - jumping or falling
            if (velocity->vy < 0) {
                animation->setState(AnimationState::JUMPING);
            } else {
                animation->setState(AnimationState::FALLING);
            }
        } else {
            auto input = entity->getComponent<InputControl>();
            if (input && input->isDown(InputKey::ATTACK)) {
                animation->setState(AnimationState::ATTACKING);
            } else if (velocity->vx != 0) {
                // On the ground - idle or walking
                animation->setState(AnimationState::WALKING);
            } else {
                animation->setState(AnimationState::IDLE);
            }
        }
    }
}
