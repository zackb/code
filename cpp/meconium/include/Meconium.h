#pragma once

#include "GameTime.h"
#include "TileMap.h"
#include "ecs/ECS.h"

class Meconium {

public:
    bool init();
    bool running() const { return isRunning; };
    void update();
    void render();
    void handleEvent();
    void shutdown();

private:
    bool isRunning = false;
    std::unique_ptr<TileMap> tileMap;

    // ecs
    std::shared_ptr<Entity> player;
    std::vector<std::shared_ptr<Entity>> entities;
    InputSystem inputSystem;
    MovementSystem movementSystem;
    RenderSystem renderSystem;
    CollisionSystem collisionSystem;
    AnimationSystem animationSystem;
    GameTime& gameTime = GameTime::getInstance();
};
