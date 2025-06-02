#pragma once

#include "GameState.h"
#include "GameTime.h"
#include "MusicManager.h"
#include "entity/Entity.h"
#include "level/Level.h"
#include "systems/AnimationSystem.h"
#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/CombatSystem.h"
#include "systems/DebugSystem.h"
#include "systems/DelayedActionSystem.h"
#include "systems/EnemyAISystem.h"
#include "systems/InputSystem.h"
#include "systems/LifecycleSystem.h"
#include "systems/MovementSystem.h"
#include "systems/PickupSystem.h"
#include "systems/ProjectileSystem.h"
#include "systems/RenderSystem.h"
#include "systems/SoundSystem.h"
#include "systems/SpawnerSystem.h"
#include "systems/StateSystem.h"
#include "systems/TweenSystem.h"
#include "systems/UIRenderSystem.h"

class Engine {
public:
    bool init();
    void run(std::unique_ptr<GameState> initialState);
    void shutdown();

    Entities& entities();
    std::shared_ptr<Entity> createEntity();
    Enemy& createEnemy();
    void loadLevel(std::string levelName);

    void enableDebug();

private:
    std::unique_ptr<GameState> state;
    std::shared_ptr<Level> level;
    std::shared_ptr<TileMap> tileMap;
    std::unique_ptr<TileMapRenderer> tileMapRenderer;
    Enemies enemies;
    MusicManager musicManager;
    SoundManager soundManager;

    // ecs
    std::shared_ptr<Entities> _entities;
    InputSystem inputSystem;
    MovementSystem movementSystem;
    SoundSystem soundSystem;
    StateSystem stateSystem;
    SpawnerSystem spawnerSystem;
    LifecycleSystem lifecycleSystem;
    ProjectileSystem projectileSystem;
    DelayedActionSystem delayedActionSystem;
    EnemyAISystem enemyAISystem;
    RenderSystem renderSystem;
    UIRenderSystem uiRenderSystem;
    CollisionSystem collisionSystem;
    CombatSystem combatSystem;
    PickupSystem pickupSystem;
    AnimationSystem animationSystem;
    TweenSystem tweenSystem;
    CameraSystem cameraSystem;
    DebugSystem debugSystem;
    GameTime& gameTime = GameTime::getInstance();

    // ecs
    void update();
    void render();
};
