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

    void addAnimation(const AnimationState state, const std::shared_ptr<Animation>& animation);

    void setState(const AnimationState state);

    void update(const int deltaTime) const;

    SDL_Rect getCurrentFrame() const;

    AnimationState getCurrentState() const {
        return currentState;
    }

private:
    std::map<AnimationState, std::shared_ptr<Animation>> animations;
    std::shared_ptr<Animation> currentAnimation;
    AnimationState currentState;
};