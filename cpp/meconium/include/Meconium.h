#include <SDL.h>
#include "ECS.h"

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
        

        // ecs
        std::vector<std::shared_ptr<Entity>> entities;
        std::vector<std::shared_ptr<Entity>> activeEntities;
        std::shared_ptr<Entity> player;
        InputSystem inputSystem;
        MovementSystem movementSystem;
};