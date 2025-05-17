#pragma once

#include "components/Component.h"
enum class EnemyBehavior { IDLE, PATROL, CHASE };

struct RangedAttack {};

struct Chase {
    int speed;
    Chase(int speed) : speed(speed) {}
};

struct Patrol {
    int left;
    int right;
    int speed;
    Patrol(int left, int right, int speed) : left(left), right(right), speed(speed) {}
};

struct EnemyAI : public Component {
    EnemyBehavior behavior;
    int cooldownMs = 0;
    int timeSinceLastAttack = 0;
};
