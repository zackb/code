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

    private:
        std::unique_ptr<TileMap> tileMap;
        bool isRunning = false;

        // ecs
        std::vector<std::shared_ptr<Entity>> entities;
        std::shared_ptr<Entity> player;
        InputSystem inputSystem;
        MovementSystem movementSystem;
        RenderSystem renderSystem;
        CollisionSystem collisionSystem;
};