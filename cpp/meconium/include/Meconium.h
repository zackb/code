#pragma once

#include "ECS.h"
#include "GameState.h"
#include "GameTime.h"
#include "level/Level.h"
#include "MusicManager.h"
#include "components/TileMap.h"
#include "systems/DebugSystem.h"
#include "systems/EnemyAISystem.h"
#include "systems/LifecycleSystem.h"
#include "systems/ProjectileSystem.h"
#include "systems/SoundSystem.h"
#include "systems/SpawnerSystem.h"
#include "systems/StateSystem.h"

class Meconium : public GameState {

public:
    bool init();
    void update() override;
    void render() override;
    void handleEvent(SDL_Event& event) override;
    std::unique_ptr<GameState> nextState() override;


private:
    std::shared_ptr<Level> level;
    std::shared_ptr<TileMap> tileMap;
    Enemies enemies;
    MusicManager musicManager;
    SoundManager soundManager;
    std::unique_ptr<GameState> next;

    // ecs
    std::shared_ptr<Entity> player;
    std::shared_ptr<Entities> entities;
    InputSystem inputSystem;
    MovementSystem movementSystem;
    SoundSystem soundSystem;
    StateSystem stateSystem;
    SpawnerSystem spawnerSystem;
    LifecycleSystem lifecycleSystem;
    ProjectileSystem projectileSystem;
    EnemyAISystem enemyAISystem;
    RenderSystem renderSystem;
    CollisionSystem collisionSystem;
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;
    DebugSystem debugSystem;
    GameTime& gameTime = GameTime::getInstance();
};
