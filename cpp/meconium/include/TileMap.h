#pragma once
#include <SDL.h>
#include <memory>
#include <sstream>
#include <fstream>

#include "ResourceManager.h"

class TileMap
{
public:
    SDL_Texture *texture = nullptr;
    int tileSize = 70;
    int mapHeight = 20;
    int mapWidth = 89;

    static std::unique_ptr<TileMap> load(SDL_Renderer* renderer, const std::string& mapPath, const std::string& texturePath)
    {
        auto tileMap = std::make_unique<TileMap>();

        std::ifstream file(mapPath);
        std::string line;
        int row = 0;

        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string val;
            std::vector<int> rowVec;

            while (std::getline(ss, val, ','))
            {
                rowVec.push_back(std::stoi(val));
            }

            tileMap->map.push_back(std::move(rowVec));
            row++;
        }

        tileMap->texture = ResourceManager::loadTexture(renderer, texturePath);
        return tileMap;
    }

    int at(int row, int col) const {
        return map.at(row).at(col);
    }

    SDL_Rect getTileSrcRect(int tileID) {
        int tilesPerRow = mapWidth / tileSize;
        int row = tileID / tilesPerRow;
        int col = tileID % tilesPerRow;
    
        SDL_Rect src = { col * tileSize, row * tileSize, tileSize, tileSize };
        return src;
    }


private:
    std::vector<std::vector<int>> map;
};