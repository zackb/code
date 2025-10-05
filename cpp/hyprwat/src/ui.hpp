#pragma once

#include "src/wayland/layer_surface.hpp"
#include "src/wayland/wayland.hpp"

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
    UI(wl::Wayland& wayland) : wayland(wayland) {}
    void init(int x, int y, int width, int height);
    void run(Frame& frame);

private:
    wl::Wayland& wayland;
    std::unique_ptr<wl::LayerSurface> surface;
    std::unique_ptr<egl::Context> egl;
    bool running = true;

    void renderFrame(Frame& frame);
};
