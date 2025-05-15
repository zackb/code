#pragma once

struct Velocity {
    int vx, vy;
    Velocity() : vx(0), vy(0) {}
    Velocity(int initVx, int initVy) : vx(initVx), vy(initVy) {}
};