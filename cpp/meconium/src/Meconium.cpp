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
    tileMap = TileMap::load("assets/map_1/map.csv", "assets/map_1/tilesheet.png");

    // Initialize ECS components, systems, and entities

    // Create a player entity
    // Load sprite
    constexpr int frameWidth = 56;
    constexpr int frameHeight = 56;
    std::shared_ptr<Sprite> sprite =
        ResourceManager::loadSprite("assets/blue_spritesheet.png", frameWidth, frameHeight);

    player = std::make_shared<Entity>(1);

    // Add velocity
    player->addComponent(std::make_shared<Velocity>(0, 0));

    // Add input control to the player
    player->addComponent(std::make_shared<InputControl>());

    // Add Collision box
    player->addComponent(std::make_shared<Collider>(15, 0, sprite->width - 30, sprite->height));

    // Add Transform
    player->addComponent(std::make_shared<Transform>(0, 0, 2.0));

    // Add Sprite
    player->addComponent<Sprite>(std::move(sprite));

    // Add animation component
    // TODO: createAnimation(sprite)?
    auto animComponent =
        ResourceManager::createPlayerAnimations("assets/blue_spritesheet.png", frameWidth, frameHeight);
    player->addComponent<AnimationComponent>(animComponent);

    entities.push_back(player);

    // add camera
    camera = std::make_shared<Entity>(2);
    camera->addComponent<Transform>(std::make_shared<Transform>(0, 0));
    camera->addComponent<Camera>(
        std::make_shared<Camera>(Context::windowSize.width, Context::windowSize.height));
    camera->addComponent<Follow>(std::make_shared<Follow>(player, 0.2f)); // smooth follow
    entities.push_back(camera);

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

#ifdef DEBUG
    auto collider = player->getComponent<Collider>();
    auto transform = player->getComponent<Transform>();
    auto camPos = camera->getComponent<Transform>();

    SDL_Rect r = collider->getBounds(transform);
    SDL_Rect hitBox = {r.x - camPos->x, r.y - camPos->y, r.w, r.h};

    SDL_SetRenderDrawColor(Context::renderer, 255, 0, 0, 255); // Red color
    SDL_RenderDrawRect(Context::renderer, &hitBox);
#endif

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
