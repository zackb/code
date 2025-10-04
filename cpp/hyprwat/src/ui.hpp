#pragma once

#include <SDL3/SDL.h>
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
    void init(std::string title);
    void run(Frame& frame);
    void destroy();

private:
    SDL_Window* window;
    SDL_GLContext glContext;
    bool running = true;
    void renderFrame(Frame& frame);
    float dpi;
    bool selfDecorate = false;
};
