#pragma once

#include "Component.h"

struct InputAction {
    SDL_Scancode key;
    bool isPressed = false;
    bool wasPressed = false;

    InputAction(SDL_Scancode scancode) : key(scancode) {}

    void update(const Uint8* keyboardState) {
        wasPressed = isPressed;
        isPressed = keyboardState[key];
    }

    bool justPressed() const { return isPressed && !wasPressed; }

    bool justReleased() const { return !isPressed && wasPressed; }
};

enum class InputKey { MOVE_LEFT, MOVE_RIGHT, JUMP };

class InputControl : public Component {
public:
    std::unordered_map<InputKey, InputAction> actions;

    InputControl() {
        actions.emplace(InputKey::MOVE_LEFT, InputAction(SDL_SCANCODE_LEFT));
        actions.emplace(InputKey::MOVE_RIGHT, InputAction(SDL_SCANCODE_RIGHT));
        actions.emplace(InputKey::JUMP, InputAction(SDL_SCANCODE_UP));
    }

    void update(const Uint8* keyboardState) {
        for (auto& [name, action] : actions) {
            action.update(keyboardState);
        }
    }

    bool isDown(const InputKey key) const {
        auto it = actions.find(key);
        return it != actions.end() && it->second.isPressed;
    }

    bool justPressed(const InputKey key) const {
        auto it = actions.find(key);
        return it != actions.end() && it->second.justPressed();
    }
};
