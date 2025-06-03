#include "corex/ui/Input.h"
#include <SDL.h>

namespace ui {

    static SDL_Event lastEvent;
    static bool hasEvent = false;

    bool pollEvent() {
        hasEvent = SDL_PollEvent(&lastEvent);
        return hasEvent;
    }

    bool keyPressed(Key key) {
        if (!hasEvent || lastEvent.type != SDL_KEYDOWN)
            return false;

        SDL_Keycode sym = lastEvent.key.keysym.sym;
        switch (key) {
        case Key::Up:
            return sym == SDLK_UP;
        case Key::Down:
            return sym == SDLK_DOWN;
        case Key::Left:
            return sym == SDLK_LEFT;
        case Key::Right:
            return sym == SDLK_RIGHT;
        case Key::Enter:
            return sym == SDLK_RETURN;
        case Key::Space:
            return sym == SDLK_SPACE;
        case Key::Escape:
            return sym == SDLK_ESCAPE;
        default:
            return false;
        }
    }

    bool quitRequested() { return hasEvent && lastEvent.type == SDL_QUIT; }

} // namespace ui
