#pragma once

#include <SDL.h>
#include <vector>
#include <string>
#include <map>
#include <memory>

enum class AnimationState {
    IDLE,
    WALKING,
    JUMPING,
    FALLING
};

struct AnimationFrame {
    SDL_Rect srcRect;
    int duration; // in milliseconds
};

class Animation {

public:

    Animation(const std::string& name, bool looping = true)
        : name(name), looping(looping), currentFrame(0), timer(0), finished(false) {}

    void addFrame(const SDL_Rect srcRect, const int duration);

    void update(const int deltaTime);

    SDL_Rect getCurrentFrame() const;

    void reset();

    bool isFinished() const {
        return finished;
    }

    const std::string& getName() const {
        return name;
    }

private:
    std::string name;
    std::vector<AnimationFrame> frames;
    bool looping;
    size_t currentFrame;
    int timer;
    bool finished;
};

class AnimationComponent {
public:
    AnimationComponent() : currentState(AnimationState::IDLE) {}

    void addAnimation(const AnimationState state, const std::shared_ptr<Animation>& animation) {
        animations[state] = animation;
        if (!currentAnimation) {
            currentAnimation = animation;
        }
    }

    void setState(const AnimationState state) {
        if (currentState != state) {
            auto it = animations.find(state);
            if (it != animations.end()) {
                currentState = state;
                currentAnimation = it->second;
                currentAnimation->reset();
            }
        }
    }

    void update(const int deltaTime) const {
        if (currentAnimation) {
            currentAnimation->update(deltaTime);
        }
    }

    SDL_Rect getCurrentFrame() const {
        if (currentAnimation) {
            return currentAnimation->getCurrentFrame();
        }
        return {0, 0, 0, 0};
    }

    AnimationState getCurrentState() const {
        return currentState;
    }

private:
    std::map<AnimationState, std::shared_ptr<Animation>> animations;
    std::shared_ptr<Animation> currentAnimation;
    AnimationState currentState;
};