#include <SDL.h>
#include <iostream>
#include <algorithm>

#include "Meconium.h"
#include "ResourceManager.h"
#include "Context.h"

Size Context::windowSize;
SDL_Renderer* Context::renderer;
SDL_Window* Context::window;

bool Meconium::init()
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    Context::window = SDL_CreateWindow("Meconium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, SDL_WINDOW_SHOWN);

    if (!Context::window)
    {
        std::cerr << "CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    Context::renderer = SDL_CreateRenderer(Context::window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!Context::renderer)
    {
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

    SDL_GetWindowSize(Context::window, 
        &Context::windowSize.width, 
        &Context::windowSize.height);


    // load tileMap
    tileMap = TileMap::load("assets/map.csv", "assets/tilesheet.png");

    // Initialize ECS components, systems, and entities
    // Create a player entity
    player = std::make_shared<Entity>(1);
    player->addComponent(std::make_shared<Position>(100, 0));// MovementSystem::groundLevel(Context::windowSize)));
    player->addComponent(std::make_shared<Velocity>(0, 0));
    player->addComponent(std::make_shared<InputControl>());

    std::shared_ptr<Sprite> sprite = ResourceManager::loadSprite("assets/player.png", 70, 70);
    player->addComponent(std::make_shared<Size>(sprite->width, sprite->height));
    player->addComponent<Sprite>(std::move(sprite));

    entities.push_back(player);

    isRunning = true;

    return true;
}

void Meconium::update()
{
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    inputSystem.update(entities, keyboardState);

    movementSystem.update(entities);
    collisionSystem.update(entities, *tileMap);

    auto& playerPos = *player->getComponent<Position>();

    // Update camera position based on player position
    Camera& camera = Camera::getInstance();
    camera.x = playerPos.x - Context::windowSize.width / 2;
    camera.y = playerPos.y - Context::windowSize.height / 2;

    // Ensure camera doesn't go out of bounds
    camera.x = std::max(0, std::min(camera.x, tileMap->mapWidth * tileMap->tileSize - Context::windowSize.width));
    camera.y = std::max(0, std::min(camera.y, tileMap->mapHeight * tileMap->tileSize - Context::windowSize.height));
}

void Meconium::render()
{
    // Set background clear color (black)
    SDL_SetRenderDrawColor(Context::renderer, 0, 0, 0, 255);
    SDL_RenderClear(Context::renderer); // Clears screen with black

    // Render entities
    renderSystem.render(entities, *tileMap);

    SDL_RenderPresent(Context::renderer);
}

void Meconium::handleEvent()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {
    case SDL_QUIT:
        isRunning = false;
    }
}

void Meconium::shutdown()
{
    SDL_DestroyRenderer(Context::renderer);
    SDL_DestroyWindow(Context::window);
    IMG_Quit();
    SDL_Quit();
}
