#include <SDL.h>
#include <memory>
#include <ostream>

#include "Context.h"
#include "Meconium.h"

#include "FileUtils.h"
#include "GameOverState.h"
#include "assets/AssetLoader.h"
#include "components/Debug.h"
#include "components/Health.h"
#include "components/State.h"

Size Context::windowSize;
SDL_Renderer* Context::renderer;
SDL_Window* Context::window;

bool Meconium::init() {
    // initialize sound effects
    soundManager.loadFromFile(resolveAssetPath("audio/sounds.json"));

    // load tileMap
    level = std::make_shared<Level>("assets/maps/level1.json");
    tileMap = level->createTileMap();

    // Initialize ECS components, systems, and entities
    entities = std::make_shared<Entities>();

    // Initialize enemies
    enemies = level->createEnemies();

    // Create a player entity
    player = std::make_shared<Entity>(1);

    // Tag as player entity
    player->addComponent<PlayerTag>();

    // Give player some health
    player->addComponent<Health>(100);

    // Load sprite
    // Add Sprite
    auto spriteDef = AssetLoader::loadSpriteSheet("assets/sprites/player.json");
    auto sprite = level->createSprite(*spriteDef);
    player->addComponent<Sprite>(sprite);

    // Add animation component
    auto animComponent = level->createAnimation(*spriteDef);
    player->addComponent<AnimationComponent>(animComponent);

    // Add velocity
    player->addComponent<Velocity>(0, 0);

    // Add input control to the player
    player->addComponent<InputControl>();

    // Add Collision box
    if (spriteDef->collider.has_value()) {
        auto rect = spriteDef->collider.value();
        player->addComponent<Collider>(rect.x, rect.y, rect.width, rect.height);
    } else {
        player->addComponent<Collider>(0, 0, sprite->width, sprite->height);
    }

    // Add Transform
    player->addComponent<Transform>(0, 0, spriteDef->scale);

    // Add State
    player->addComponent<State>();

    // Add player to the entities list
    entities->add(player);

    // add camera
    auto camera = std::make_shared<Entity>(2);
    camera->addComponent<Transform>(0, 0);
    camera->addComponent<Camera>(Context::windowSize.width, Context::windowSize.height);
    camera->addComponent<Follow>(player, 0.2f); // smooth follow
    entities->add(camera);

    // add parallax background
    auto bk = std::make_shared<Entity>(3);
    bk->addComponent<ParallaxBackground>(level->createBackground());
    entities->add(bk);

    // add debugging
    auto debug = std::make_shared<Entity>(4);
    debug->addComponent<Debug>();
    debug->addComponent<InputControl>();
    entities->add(debug);

    // start background music
    if (!level->getBackgroundMusic().empty()) {
        musicManager.load(level->getBackgroundMusic());
        musicManager.play(-1);
    }

    return true;
}

void Meconium::update() {
    // Update game time
    gameTime.update();
    int deltaTime = gameTime.getDeltaTime();

    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

    // Handle input first to affect movement
    inputSystem.update(entities, keyboardState);

    // Apply movement based on input
    movementSystem.update(entities);

    // Play sounds if we should
    soundSystem.update(entities, soundManager);

    // Handle enemy AI
    enemyAISystem.update(entities, level, deltaTime);

    // Transition state
    stateSystem.update(entities, deltaTime);

    // Handle collisions after movement
    collisionSystem.update(entities, *tileMap);

    // Update animations based on state
    animationSystem.update(entities, deltaTime);

    // Update projectile status
    projectileSystem.update(entities, deltaTime);

    // Update camera after movement and collision
    cameraSystem.update(entities, *tileMap);

    // Spawn enemies if we should
    spawnerSystem.update(entities, enemies, level);

    // Entity lifecycle check
    lifecycleSystem.update(entities, deltaTime);

    // Check for player death
    if (!entities->findEntityWithComponent<PlayerTag>()) {
        next = std::make_unique<GameOverState>();
    }
}

void Meconium::render() {
    // Set background clear color (black)
    SDL_SetRenderDrawColor(Context::renderer, 0, 0, 0, 255);
    SDL_RenderClear(Context::renderer); // Clears screen with black

    // Render entities
    renderSystem.render(entities, *tileMap);

    // Check debugging
    debugSystem.update(entities, tileMap);

    SDL_RenderPresent(Context::renderer);
}

void Meconium::handleEvent(SDL_Event& event) {
    // TODO: do we need this?
}

std::unique_ptr<GameState> Meconium::nextState() {
    return std::move(next); // Return ownership of next state if set
}