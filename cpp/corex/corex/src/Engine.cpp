#include "corex/Engine.h"
#include "corex/Context.h"
#include "corex/components/Debug.h"
#include "corex/entity/EntityFactory.h"
#include "corex/ui/Input.h"
#include <SDL.h>
#include <memory>

bool Engine::init() {

    // initialize SDL
    if (!Context::init()) {
        return false;
    }

    // Initialize ECS components, systems, and entities
    _entities = std::make_shared<Entities>();

    // initialize sound effects
    soundManager.loadFromFile(resolveAssetPath("audio/sounds.json"));

    return true;
}

Entities& Engine::entities() { return *_entities; }

void Engine::loadLevel(std::string levelName) {

    // load tileMap
    level = std::make_shared<Level>("assets/maps/" + levelName + ".json");
    tileMap = level->createTileMap();
    tileMapRenderer = std::make_unique<TileMapRenderer>(*tileMap, Context::renderer, 16);

    // Initialize enemies
    enemies = level->createEnemies();

    // Add interactables
    for (auto i : level->getInteractableDefinitions()) {
        _entities->add(EntityFactory::createInteractable(i));
    }

    // add parallax background
    auto bk = createEntity();
    bk->addComponent<ParallaxBackground>(level->createBackground());

    // start background music
    if (!level->getBackgroundMusic().empty()) {
        musicManager.load(level->getBackgroundMusic());
        musicManager.play(-1);
    }
}

std::shared_ptr<Entity> Engine::createEntity() {
    auto entity = std::make_shared<Entity>();
    _entities->add(entity);
    return entity;
}

void Engine::unload() {
    _entities->clear();
    level = nullptr;
    tileMap = nullptr;
    musicManager.stop();
}

void Engine::run(std::unique_ptr<GameState> initialState) {
    state = std::move(initialState);

    bool isRunning = true;

    constexpr int FPS = 60;
    constexpr int frameDelay = 1000 / FPS;

    Context::avgFPS = FPS;     // start at target FPS
    const double alpha = 0.05; // smoothing factor for EMA

    while (isRunning) {
        const Uint32 frameStart = SDL_GetTicks();

        ui::Input::beginFrame();

        while (ui::Input::pollEvent()) {
            if (ui::Input::quitRequested()) {
                isRunning = false;
            }

            state->handleEvent();
        }

        state->update(gameTime.getDeltaTime());
        update();
        render();
        state->render();

        // Transition to the next state if needed
        if (std::unique_ptr<GameState> next = state->nextState()) {
            state = std::move(next);
        }

        Uint32 frameEnd = SDL_GetTicks();
        Uint32 frameTime = frameEnd - frameStart;

        // Update running average FPS
        double currentFPS = (frameTime > 0) ? (1000.0 / frameTime) : 0.0;
        Context::avgFPS = (1.0 - alpha) * Context::avgFPS + alpha * currentFPS;

        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void Engine::update() {
    if (!_entities || _entities->empty()) {
        return;
    }

    // Update game time
    gameTime.update();
    int deltaTime = gameTime.getDeltaTime();

    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

    // Handle input first to affect movement
    inputSystem.update(*_entities, keyboardState);

    // Apply movement based on input
    movementSystem.update(*_entities);

    // Play sounds if we should
    soundSystem.update(*_entities, soundManager);

    // Handle enemy AI
    enemyAISystem.update(_entities, deltaTime);

    // Transition state
    stateSystem.update(_entities, deltaTime);

    // Handle combat collisions
    combatSystem.update(_entities);

    // Handle collisions after movement
    collisionSystem.update(*_entities, *tileMap);

    // Handle interactions (pickups, doors, etc)
    interactionSystem.update(*_entities);

    // Update animations based on state
    animationSystem.update(*_entities, deltaTime);

    // Update tweens
    tweenSystem.update(*_entities, deltaTime);

    // Update projectile status
    projectileSystem.update(*_entities, deltaTime);

    // Handle delayed actions
    delayedActionSystem.update(*_entities, deltaTime);

    // Update camera after movement and collision
    cameraSystem.update(*_entities, *tileMap);

    // Spawn enemies if we should
    spawnerSystem.update(*_entities, enemies);

    // Entity lifecycle check
    lifecycleSystem.update(*_entities, deltaTime);
}

void Engine::render() {
    if (!_entities || _entities->empty()) {
        return;
    }

    // Set background clear color (black)
    SDL_SetRenderDrawColor(Context::renderer, 0, 0, 0, 255);
    SDL_RenderClear(Context::renderer); // Clears screen with black

    // Render background, tilemap, and entities
    renderSystem.render(*_entities, *tileMapRenderer);

    // Render UI
    uiRenderSystem.render(*_entities);

    // Check debugging
    debugSystem.update(*_entities, *tileMap);

    SDL_RenderPresent(Context::renderer);
    const char* err = SDL_GetError();
    if (err && err[0] != '\0') {
        printf("SDL error: %s\n", err);
        SDL_ClearError();
    }
}

void Engine::enableDebug() {
    // add debugging
    auto debug = std::make_shared<Entity>(4);
    debug->addComponent<Debug>();
    debug->addComponent<InputControl>();
    _entities->add(debug);
}

void Engine::shutdown() { Context::destroy(); }
