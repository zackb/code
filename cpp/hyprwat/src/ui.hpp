#pragma once

#include "imgui.h"
#include <SDL_video.h>
#include <string>

class Frame {
public:
    virtual bool render() = 0;
};

class UI {
public:
    void init(std::string title);
    void run(Frame& frame);
    void destroy();

private:
    SDL_Window* window;
    SDL_GLContext gl_context;
    bool running = true;
    void renderFrame(Frame& frame);
    void resizeWindow(const ImVec2& contentSize);
    float dpi;
};
