#pragma once

struct Despawn {
    int timer = 0;
    Despawn() {}
    Despawn(int timer) : timer(timer) {}
};
