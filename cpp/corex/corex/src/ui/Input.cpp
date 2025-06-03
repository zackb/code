#include "corex/ui/Input.h"
#include <SDL.h>
#include <unordered_set>

namespace ui {

    void Input::beginFrame() {
        // Clear state at frame start
        while (!eventQueue.empty())
            eventQueue.pop();
        quit = false;
        pressedKeys.clear();
        // don't clear downKeys here, keep keys held across frames
    }

    bool Input::pollEvent() {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            eventQueue.push(event);

            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                pressedKeys.insert(event.key.keysym.sym);
                downKeys.insert(event.key.keysym.sym);
            } else if (event.type == SDL_KEYUP) {
                downKeys.erase(event.key.keysym.sym);
            }
            return true;
        }
        return false;
    }

    bool Input::quitRequested() { return quit; }

    bool Input::keyPressed(Key key) { return pressedKeys.count(toSDLKey(key)) > 0; }

    bool Input::keyDown(Key key) { return downKeys.count(toSDLKey(key)) > 0; }

    SDL_Keycode Input::toSDLKey(Key key) {
        switch (key) {
        case Key::Up:
            return SDLK_UP;
        case Key::Down:
            return SDLK_DOWN;
        case Key::Left:
            return SDLK_LEFT;
        case Key::Right:
            return SDLK_RIGHT;
        case Key::Enter:
            return SDLK_RETURN;
        case Key::Space:
            return SDLK_SPACE;
        case Key::Escape:
            return SDLK_ESCAPE;
        default:
            return SDLK_UNKNOWN;
        }
    }

} // namespace ui
