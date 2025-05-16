#pragma once

enum class EnemyBehavior { IDLE, PATROL, CHASE };

struct Patrol {
    int left;
    int right;
    int speed;
    Patrol(int left, int right, int speed) : left(left), right(right), speed(speed) {}
};
