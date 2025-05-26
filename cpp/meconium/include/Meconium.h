#pragma once

#include "ECS.h"
#include "GameState.h"
#include "GameTime.h"
#include "MusicManager.h"
#include "components/TileMap.h"
#include "level/Level.h"
#include "systems/CombatSystem.h"
#include "systems/DebugSystem.h"
#include "systems/DelayedActionSystem.h"
#include "systems/EnemyAISystem.h"
#include "systems/LifecycleSystem.h"
#include "systems/PickupSystem.h"
#include "systems/ProjectileSystem.h"
#include "systems/SoundSystem.h"
#include "systems/SpawnerSystem.h"
#include "systems/StateSystem.h"
#include "systems/UIRenderSystem.h"

class Meconium : public GameState {

public:
    bool init(std::string character = "blue");
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
    DelayedActionSystem delayedActionSystem;
    EnemyAISystem enemyAISystem;
    RenderSystem renderSystem;
    UIRenderSystem uiRenderSystem;
    CollisionSystem collisionSystem;
    CombatSystem combatSystem;
    PickupSystem pickupSystem;
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;
    DebugSystem debugSystem;
    GameTime& gameTime = GameTime::getInstance();
};
