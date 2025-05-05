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
                auto position = entity->getComponent<Position>();
                auto sprite = entity->getComponent<Sprite>();
                if (!position)
                {
                    continue;
                }

                if (sprite)
                {
                    auto& pos = *position;
    
                    SDL_Rect dstRect = {
                        static_cast<int>(pos.x),
                        static_cast<int>(pos.y),
                        sprite->width,
                        sprite->height
                    };
    
                    SDL_RenderCopy(renderer, sprite->texture, NULL, &dstRect);
                }
                else
                {
                    SDL_Rect entityRect = {position->x, position->y, 50, 50};
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    SDL_RenderFillRect(renderer, &entityRect);
                }
            }
        }
};