#pragma once

#include "components/Component.h"
#include <functional>

enum class Action {
    // Shared
    IDLE,
    WALKING,
    JUMPING,
    FALLING,
    ATTACKING,
    COLLECTING,
    DYING,
};

struct State final : Component {
    Action currentAction = Action::IDLE;

    bool isActionLocked = false;

    // how long the current action has lasted
    int actionTimeMs = 0;

    // total duration of the current locked action (sword swing)
    int actionDurationMs = 0;

    bool facingRight = true;

    // if the result of this action has happened (attack landed)
    bool actionApplied = false;

    std::function<void()> onUnlock = nullptr;

    void lockAction(const Action action, const int duration, std::function<void()> callback = nullptr) {
        isActionLocked = true;
        currentAction = action;
        actionDurationMs = duration;
        actionTimeMs = 0;
        actionApplied = false;
        onUnlock = callback;
    }

    // check if we need to unlock the action and clear the locked state
    void checkActionLock() {
        if (actionTimeMs >= actionDurationMs) {
            isActionLocked = false;
            if (onUnlock) {
                onUnlock();
                onUnlock = nullptr; // clear it after use
            }
        }
    }
};
