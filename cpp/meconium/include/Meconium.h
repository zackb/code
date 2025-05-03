#include <SDL.h>

class Meconium 
{
    public:
        bool init();
        void run();
        void shutdown();

    private:
        SDL_Window *window;
        SDL_Renderer *renderer;

};