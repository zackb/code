#pragma once

enum class EnemyBehavior { IDLE, PATROL, CHASE };

struct Patrol {
    int boundsRight;
    int boundsLeft;
};
