#pragma once

#include <SDL.h>
#include <SDL_image.h>

class RenderSystem
{
    public:
        void render(SDL_Renderer *renderer, const std::vector<std::shared_ptr<Entity>> &entities)
        {
            for (auto entity : entities)
            {
                if (!entity->hasPosition())
                    continue;

                if (entity->hasSprite())
                {
                    // render sprite
                }
                else
                {
                    SDL_Rect entityRect = {entity->position->x, entity->position->y, 50, 50};
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    SDL_RenderFillRect(renderer, &entityRect);
                }
            }
        }
};