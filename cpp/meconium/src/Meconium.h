#include <SDL.h>

class Meconium 
{
    public:
        bool init();
        bool running() { return isRunning; };
        void update();
        void render();
        void handleEvent();
        void shutdown();

    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        bool isRunning = false;
};