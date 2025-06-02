#pragma once

#include "Component.h"
#include <SDL.h>

#include <SDL_scancode.h>
#include <unordered_map>
#include <vector>

struct InputAction {
    SDL_Scancode key;
    bool isPressed = false;
    bool wasPressed = false;
    std::vector<SDL_Scancode> requiredModifiers;

    InputAction(SDL_Scancode scancode) : key(scancode) {}

    void addModifier(SDL_Scancode mod) { requiredModifiers.push_back(mod); }

    void update(const Uint8* keyboardState) {
        wasPressed = isPressed;
        bool keyDown = keyboardState[key];

        // all modifiers must be down
        bool modifiersHeld = true;
        for (SDL_Scancode code : requiredModifiers) {
            if (!keyboardState[code]) {
                modifiersHeld = false;
            }
        }
        isPressed = keyDown && modifiersHeld;
    }

    bool justPressed() const { return isPressed && !wasPressed; }

    bool justReleased() const { return !isPressed && wasPressed; }
};

enum class InputKey { MOVE_LEFT, MOVE_RIGHT, JUMP, ATTACK, DEBUG };

class InputControl final : public Component {
public:
    std::unordered_map<InputKey, InputAction> actions;

    InputControl() {

        // move
        actions.emplace(InputKey::MOVE_LEFT, InputAction(SDL_SCANCODE_LEFT));
        actions.emplace(InputKey::MOVE_RIGHT, InputAction(SDL_SCANCODE_RIGHT));
        actions.emplace(InputKey::JUMP, InputAction(SDL_SCANCODE_UP));

        // actions
        actions.emplace(InputKey::ATTACK, InputAction(SDL_SCANCODE_SPACE));

        // debug command
        InputAction debugAction(SDL_SCANCODE_D);
        debugAction.addModifier(SDL_SCANCODE_LGUI);
        actions.emplace(InputKey::DEBUG, std::move(debugAction));
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
