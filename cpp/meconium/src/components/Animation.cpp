#include "components/Animation.h"

void Animation::addFrame(const SDL_Rect srcRect, const int duration) { frames.push_back({srcRect, duration}); }

void Animation::update(const int deltaTime) {
    if (frames.empty() || finished)
        return;

    timer += deltaTime;

    if (timer >= frames[currentFrame].duration) {
        timer -= frames[currentFrame].duration;
        currentFrame++;

        if (currentFrame >= frames.size()) {
            if (looping) {
                currentFrame = 0;
            } else {
                currentFrame = frames.size() - 1;
                finished = true;
            }
        }
    }
}

SDL_Rect Animation::getCurrentFrame() const {
    if (frames.empty()) {
        return {0, 0, 0, 0};
    }
    return frames[currentFrame].srcRect;
}

void Animation::reset() {
    currentFrame = 0;
    timer = 0;
    finished = false;
}

void AnimationComponent::addAnimation(const AnimationState state, const std::shared_ptr<Animation>& animation) {
    animations[state] = animation;
    if (!currentAnimation) {
        currentAnimation = animation;
    }
}
void AnimationComponent::setState(const AnimationState state) {
    if (currentState != state || !currentAnimation) {
        auto it = animations.find(state);
        if (it != animations.end()) {
            currentState = state;
            currentAnimation = it->second;
            currentAnimation->reset();
        }
    }
}

void AnimationComponent::update(const int deltaTime) const {
    if (currentAnimation) {
        currentAnimation->update(deltaTime);
    }
}

SDL_Rect AnimationComponent::getCurrentFrame() const {
    if (currentAnimation) {
        return currentAnimation->getCurrentFrame();
    }
    return {0, 0, 0, 0};
}

void AnimationComponent::init() {
    currentAnimation = nullptr; // hack: currentAnimation is set in addAnimation
    setState(AnimationState::IDLE);
}
