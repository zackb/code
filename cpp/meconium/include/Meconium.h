#pragma once

#include <SDL.h>
#include "ecs/ECS.h"
#include "Size.h"
#include "TileMap.h"

class Meconium 
{
    public:
        bool init();
        bool running() { return isRunning; };
        void update();
        void render();
        void handleEvent();
        void shutdown();
        static Size windowSize;

    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        std::unique_ptr<TileMap> tileMap;
        bool isRunning = false;
        

        // ecs
        std::vector<std::shared_ptr<Entity>> entities;
        std::vector<std::shared_ptr<Entity>> activeEntities;
        InputSystem inputSystem;
        MovementSystem movementSystem;
        RenderSystem renderSystem;
};