#pragma once

#include "components/Component.h"

enum class Action {
    // Shared
    IDLE,
    WALKING,
    JUMPING,
    FALLING,
    ATTACKING,
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
};
