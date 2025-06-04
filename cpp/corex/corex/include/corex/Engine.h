#pragma once

#include "corex/GameState.h"
#include "corex/GameTime.h"
#include "corex/MusicManager.h"
#include "corex/entity/Entity.h"
#include "corex/level/Level.h"
#include "corex/systems/AnimationSystem.h"
#include "corex/systems/CameraSystem.h"
#include "corex/systems/CollisionSystem.h"
#include "corex/systems/CombatSystem.h"
#include "corex/systems/DebugSystem.h"
#include "corex/systems/DelayedActionSystem.h"
#include "corex/systems/EnemyAISystem.h"
#include "corex/systems/InputSystem.h"
#include "corex/systems/InteractionSystem.h"
#include "corex/systems/LifecycleSystem.h"
#include "corex/systems/MovementSystem.h"
#include "corex/systems/ProjectileSystem.h"
#include "corex/systems/RenderSystem.h"
#include "corex/systems/SoundSystem.h"
#include "corex/systems/SpawnerSystem.h"
#include "corex/systems/StateSystem.h"
#include "corex/systems/TweenSystem.h"
#include "corex/systems/UIRenderSystem.h"

class Engine {
public:
    bool init();
    void run(std::unique_ptr<GameState> initialState);
    void shutdown();

    Entities& entities();
    std::shared_ptr<Entity> createEntity();
    Enemy& createEnemy();
    void loadLevel(std::string levelName);
    void unload();

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
    InteractionSystem interactionSystem;
    AnimationSystem animationSystem;
    TweenSystem tweenSystem;
    CameraSystem cameraSystem;
    DebugSystem debugSystem;
    GameTime& gameTime = GameTime::getInstance();

    // ecs
    void update();
    void render();
};
