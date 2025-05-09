#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>
#include <iostream>

#include "Context.h"
#include "Meconium.h"
#include "ResourceManager.h"

Size Context::windowSize;
SDL_Renderer* Context::renderer;
SDL_Window* Context::window;

bool Meconium::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    Context::window =
        SDL_CreateWindow("Meconium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, SDL_WINDOW_SHOWN);

    if (!Context::window) {
        std::cerr << "CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    Context::renderer = SDL_CreateRenderer(Context::window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!Context::renderer) {
        std::cerr << "CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(Context::window);
        SDL_Quit();
        return false;
    }

    // init sdl2_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << std::endl;
        return false;
    }

    SDL_GetWindowSize(Context::window, &Context::windowSize.width, &Context::windowSize.height);

    // load tileMap
    tileMap = TileMap::load("assets/map.csv", "assets/tilesheet.png");

    // Initialize ECS components, systems, and entities
    // Create a player entity
    player = std::make_shared<Entity>(1);
    player->addComponent(std::make_shared<Position>(100, 0)); // MovementSystem::groundLevel(Context::windowSize)));
    player->addComponent(std::make_shared<Velocity>(0, 0));
    player->addComponent(std::make_shared<InputControl>());

    // Create player sprite and animations
    /* Urchin
    constexpr int frameWidth = 70;
    constexpr int frameHeight = 70;
    std::shared_ptr<Sprite> sprite = ResourceManager::loadSpriteScaled("assets/player.png", 70, 70);
    */

    constexpr int frameWidth = 56;
    constexpr int frameHeight = 56;
    std::shared_ptr<Sprite> sprite =
        ResourceManager::loadSprite("assets/blue_spritesheet.png", frameWidth, frameHeight, 2.0);
    player->addComponent(std::make_shared<Size>(sprite->width * sprite->scale, sprite->height * sprite->scale));
    player->addComponent<Sprite>(std::move(sprite));

    // Add animation component
    auto animComponent =
        ResourceManager::createPlayerAnimations("assets/blue_spritesheet.png", frameWidth, frameHeight);
    player->addComponent<AnimationComponent>(animComponent);

    entities.push_back(player);

    // add camera
    auto cameraEntity = std::make_shared<Entity>(2);
    cameraEntity->addComponent<Position>(std::make_shared<Position>(0, 0));
    cameraEntity->addComponent<CameraComponent>(std::make_shared<CameraComponent>(Context::windowSize.width, Context::windowSize.height));
    cameraEntity->addComponent<FollowComponent>(std::make_shared<FollowComponent>(player, 0.2f)); // smooth follow
    entities.push_back(cameraEntity);

    isRunning = true;

    return true;
}

void Meconium::update() {
    // Update game time
    gameTime.update();
    int deltaTime = gameTime.getDeltaTime();

    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    inputSystem.update(entities, keyboardState);

    movementSystem.update(entities);
    collisionSystem.update(entities, *tileMap);
    animationSystem.update(entities, deltaTime);
    cameraSystem.update(entities, *tileMap);
}

void Meconium::render() {
    // Set background clear color (black)
    SDL_SetRenderDrawColor(Context::renderer, 0, 0, 0, 255);
    SDL_RenderClear(Context::renderer); // Clears screen with black

    // Render entities
    renderSystem.render(entities, *tileMap);

    SDL_RenderPresent(Context::renderer);
}

void Meconium::handleEvent() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        isRunning = false;
    }
}

void Meconium::shutdown() {
    ResourceManager::cleanup();
    SDL_DestroyRenderer(Context::renderer);
    SDL_DestroyWindow(Context::window);
    IMG_Quit();
    SDL_Quit();
}
