#pragma once

#include "ECS.h"
#include "GameTime.h"
#include "components/TileMap.h"

// #define DEBUG

class Meconium {

public:
    bool init();
    bool running() const { return isRunning; };
    void update();
    void render();
    void handleEvent();
    void shutdown() const;

private:
    bool isRunning = false;
    std::shared_ptr<TileMap> tileMap;

    // ecs
    std::shared_ptr<Entity> player;
    // TODO: remove after moving Debug to component
    std::shared_ptr<Entity> camera;
    std::shared_ptr<Entities> entities;
    InputSystem inputSystem;
    MovementSystem movementSystem;
    RenderSystem renderSystem;
    CollisionSystem collisionSystem;
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;
    GameTime& gameTime = GameTime::getInstance();
};
