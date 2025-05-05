#include "Meconium.h"
#include "ResourceManager.h"
#include <SDL.h>
#include <iostream>


bool Meconium::init()
{

    // Initialize ECS components, systems, and entities
    // Create a player entity
    std::shared_ptr<Entity> player = std::make_unique<Entity>(1);
    player->addPosition(100, 100);
    player->addVelocity(0, 0);
    player->addInputControl();

    std::shared_ptr<Sprite> sprite = std::make_unique<Sprite>();
    sprite->texture = ResourceManager::loadTexture(renderer, "assets/player.png");
    player->addSprite(std::move(sprite));

    entities.push_back(player);

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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    isRunning = true;

    return true;
}

void Meconium::update()
{
    movementSystem.update(entities);
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
    default:
        inputSystem.update(entities, event);
    }
}

void Meconium::shutdown()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}