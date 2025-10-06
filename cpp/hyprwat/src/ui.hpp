#pragma once

#include "src/vec2.hpp"
#include "src/wayland/layer_surface.hpp"
#include "src/wayland/wayland.hpp"

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
    float scale = 2.0f;
    bool running = true;

    void renderFrame(Frame& frame);
};
