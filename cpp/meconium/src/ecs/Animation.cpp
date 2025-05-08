#include "../../include/ecs/Animation.h"

void Animation::addFrame(const SDL_Rect srcRect, const int duration) {
    frames.push_back({srcRect, duration});
}

void Animation::update(const int deltaTime) {
    if (frames.empty() || finished) return;

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
