#pragma once
#include <SDL.h>
#include <memory>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <unordered_set>

#include "ResourceManager.h"

class TileMap
{
public:
    SDL_Texture *texture = nullptr;
    int tileSize = 70;
    int mapHeight = 0; // rows
    int mapWidth = 0; // cols
    int tilesheetWidth = 0;  // Width of tilesheet in pixels


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

        // Query texture width
        int w = 0, h = 0;
        SDL_QueryTexture(tileMap->texture, NULL, NULL, &w, &h);
        tileMap->tilesheetWidth = w;

        return tileMap;
    }

    int at(int row, int col) const {
        return map.at(row).at(col);
    }

    SDL_Rect getTileSrcRect(int tileID) {
        int tilesPerRow = tilesheetWidth / tileSize;
        int row = tileID / tilesPerRow;
        int col = tileID % tilesPerRow;

        SDL_Rect src = { col * tileSize, row * tileSize, tileSize, tileSize };
        return src;
    }

    bool isSolidTile(int tileID) const {
        return solidTiles.count(tileID) > 0;
    }



private:
    std::vector<std::vector<int>> map;
    std::unordered_set<int> solidTiles = {0, 33, 7, 18, 28, 25};
};