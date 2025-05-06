#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "TileMap.h"

#include "Size.h"

class RenderSystem
{
public:
    void render(SDL_Renderer *renderer, const std::vector<std::shared_ptr<Entity>> &entities, TileMap& tileMap, Size &windowSize)
    {
        renderTileMap(renderer, tileMap, windowSize);

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

                SDL_Rect dstRect = {
                    static_cast<int>(pos.x),
                    static_cast<int>(pos.y),
                    sprite->width,
                    sprite->height};

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

private:
    void renderTileMap(SDL_Renderer *renderer, TileMap& tileMap, Size& windowSize)
    {
        for (int row = 0; row < tileMap.mapHeight; ++row)
        {
            for (int col = 0; col < tileMap.mapWidth; ++col)
            {
                int tileID = tileMap.at(row, col);
                if (tileID < 0)
                    continue;

                SDL_Rect src = tileMap.getTileSrcRect(tileID);
                SDL_Rect dst = {
                    col * tileMap.tileSize - camera.x,
                    row * tileMap.tileSize - camera.y,
                    tileMap.tileSize,
                    tileMap.tileSize};

                SDL_RenderCopy(renderer, tileMap.texture, &src, &dst);
            }
        }
    }
};