#pragma once

#include "ECS.h"
#include "GameTime.h"
#include "components/TileMap.h"
#include "systems/DebugSystem.h"
#include "systems/StateSystem.h"

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
    std::shared_ptr<Entities> entities;
    InputSystem inputSystem;
    MovementSystem movementSystem;
    StateSystem stateSystem;
    RenderSystem renderSystem;
    CollisionSystem collisionSystem;
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;
    DebugSystem debugSystem;
    GameTime& gameTime = GameTime::getInstance();
};
