#include "Meconium.h"
#include <SDL.h>
#include <iostream>


bool Meconium::init()
{

    // Initialize ECS components, systems, and entities

    // Create a player entity
    player = std::make_unique<Entity>(1);
    player->addPosition(100, 100);
    player->addVelocity(0, 0);
    player->addInputControl();
    entities.push_back(player);

    // Create input and movement systems
    inputSystem = std::make_unique<InputSystem>();
    movementSystem = std::make_unique<MovementSystem>();


    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        "Meconium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
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
    movementSystem->update(entities);
}

void Meconium::render()
{
    // Set background clear color (black)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer); // Clears screen with black

    // Render player
    for (auto &entity : entities)
    {
        if (entity->hasPosition())
        {
            SDL_Rect playerRect = {entity->position->x, entity->position->y, 50, 50};
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Red color
            SDL_RenderFillRect(renderer, &playerRect);
        }
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
        inputSystem->update(entities, event);
    }
}

void Meconium::shutdown()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}