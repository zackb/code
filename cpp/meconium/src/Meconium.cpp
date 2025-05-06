#include "Meconium.h"
#include "ResourceManager.h"
#include <SDL.h>
#include <iostream>


Size Meconium::windowSize;

bool Meconium::init()
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        "Meconium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        std::cerr << "CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // init sdl2_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << std::endl;
        return false;
    }

    SDL_GetWindowSize(window, 
        &Meconium::windowSize.width, 
        &Meconium::windowSize.height);

    // Initialize ECS components, systems, and entities
    // Create a player entity
    std::shared_ptr<Entity> player = std::make_shared<Entity>(1);
    player->addComponent(std::make_shared<Position>(100, MovementSystem::groundLevel(Meconium::windowSize)));
    player->addComponent(std::make_shared<Velocity>(0, 0));
    player->addComponent(std::make_shared<InputControl>());

    std::shared_ptr<Sprite> sprite = ResourceManager::loadSprite(renderer, "assets/player.png", 100, 100);
    player->addComponent<Sprite>(std::move(sprite));

    entities.push_back(player);

    isRunning = true;

    return true;
}

void Meconium::update()
{
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    inputSystem.update(entities, keyboardState);

    movementSystem.update(entities, Meconium::windowSize);
}

void Meconium::render()
{
    // Set background clear color (black)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer); // Clears screen with black

    // Render player
    for (auto &entity : entities)
    {
        renderSystem.render(renderer, entities);
    }

    SDL_RenderPresent(renderer);
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
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}