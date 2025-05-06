#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "TileMap.h"
#include "Context.h"
#include "Camera.h"

#include "Size.h"

class RenderSystem
{
public:
    void render(const std::vector<std::shared_ptr<Entity>> &entities, TileMap& tileMap)
    {
        Camera& camera = Camera::getInstance();

        renderTileMap(tileMap, camera);

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
                auto &pos = *position;

                SDL_Rect dstRect;
                dstRect.x = pos.x - camera.x;  // Apply camera offset
                dstRect.y = pos.y - camera.y;  // Apply camera offset
                dstRect.w = sprite->width;
                dstRect.h = sprite->height;

                SDL_RenderCopy(Context::renderer, sprite->texture, NULL, &dstRect);
            }
            else
            {
                SDL_Rect entityRect = {position->x, position->y, 50, 50};
                SDL_SetRenderDrawColor(Context::renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(Context::renderer, &entityRect);
            }
        }
    }

private:
    void renderTileMap(TileMap& tileMap, Camera& camera)
    {
        for (int row = 0; row < tileMap.mapHeight; ++row)
        {
            for (int col = 0; col < tileMap.mapWidth; ++col)
            {
                int tileID = tileMap.at(row, col);
                if (tileID < 0)
                    continue;

                std::cout << "Camera x y " << camera.x << " " << camera.y << std::endl;
                SDL_Rect src = tileMap.getTileSrcRect(tileID);
                SDL_Rect dst = {
                    col * tileMap.tileSize - camera.x,
                    row * tileMap.tileSize - camera.y,
                    tileMap.tileSize,
                    tileMap.tileSize};

                SDL_RenderCopy(Context::renderer, tileMap.texture, &src, &dst);
            }
        }
    }
};