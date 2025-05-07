#pragma once
#include <SDL.h>
#include <memory>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "ResourceManager.h"

class TileMap
{
public:
    SDL_Texture *texture = nullptr;
    int tileSize = 70;
    int mapHeight = 0;
    int mapWidth = 0;

    static std::unique_ptr<TileMap> load(const std::string& mapPath, const std::string& texturePath)
    {
        auto tileMap = std::make_unique<TileMap>();
    
        std::string realPath = mapPath;
        if (!std::filesystem::exists(realPath)) {
            realPath = "../Resources/" + mapPath;
        }

        std::ifstream file(realPath);
        std::string line;
    
        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string val;
            std::vector<int> rowVec;
    
            while (std::getline(ss, val, ','))
            {
                rowVec.push_back(std::stoi(val));
            }
    
            if (tileMap->mapWidth == 0) {
                tileMap->mapWidth = rowVec.size();
            } else if (rowVec.size() != tileMap->mapWidth) {
                throw std::runtime_error("Inconsistent row width in map file");
            }
    
            tileMap->map.push_back(std::move(rowVec));
        }
    
        tileMap->mapHeight = tileMap->map.size();
        tileMap->texture = ResourceManager::loadTexture(texturePath);
        return tileMap;
    }

    int at(int row, int col) const {
        return map.at(row).at(col);
    }

    SDL_Rect getTileSrcRect(int tileID) {
        int row = tileID / mapWidth;
        int col = tileID % mapWidth;
    
        SDL_Rect src = { col * tileSize, row * tileSize, tileSize, tileSize };
        return src;
    }


private:
    std::vector<std::vector<int>> map;
};