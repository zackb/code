#pragma once

#include "src/wayland/wayland.hpp"
#include <string>

struct Vec2 {
    float x;
    float y;
};

class Frame {
public:
    virtual bool render() = 0;
    virtual Vec2 getSize() = 0;
};

class UI {
public:
    void init(wl::Wayland& wayland);
    void run(Frame& frame);
    void destroy();

private:
    bool running = true;
    void renderFrame(Frame& frame);
    float dpi;
    bool selfDecorate = false;
};
