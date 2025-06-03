#pragma once

#include <SDL.h>
#include <queue>
#include <unordered_set>

namespace ui {

    enum class Key {
        Up,
        Down,
        Left,
        Right,
        Enter,
        Space,
        Escape,
    };

    class Input {
    public:
        // Call at the start of each frame to reset states
        static void beginFrame();

        // Polls one SDL event internally, returns true if event was available
        static bool pollEvent();

        // Returns true if SDL_QUIT was requested during polling
        static bool quitRequested();

        // Returns true if this key was pressed during this frame
        static bool keyPressed(Key key);

        // Returns true if this key is currently down
        static bool keyDown(Key key);

    private:
        // Converts Key to SDL_Keycode
        static SDL_Keycode toSDLKey(Key key);

        // Event queue to store events polled
        inline static std::queue<SDL_Event> eventQueue;

        inline static bool quit = false;

        // Keyboard state tracking for this frame
        inline static std::unordered_set<SDL_Keycode> pressedKeys;
        inline static std::unordered_set<SDL_Keycode> downKeys;
    };
} // namespace ui
