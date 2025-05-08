#pragma once
#include <SDL.h>
#include <memory>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <unordered_set>

#include "ResourceManager.h"

enum class TileType {
    Empty,
    Solid,
    RampLeft,
    RampRight
};

class TileMap {

public:
    SDL_Texture *texture = nullptr;
    int tileSize = 70;
    int mapHeight = 0; // rows
    int mapWidth = 0; // cols
    int tilesheetWidth = 0; // Width of tilesheet in pixels

    static std::unique_ptr<TileMap> load(const std::string mapPath, std::string texturePath);

    int at(const int row, const int col) const {
        return map.at(row).at(col);
    }

    SDL_Rect getTileSrcRect(int tileID);

    bool isSolidTile(int tileID) const {
        return solidTiles.count(tileID) > 0;
    }

    TileType getTileType(int tileID) const {
        if (tileID == 0) return TileType::Solid;
        if (tileID == 7) return TileType::Solid;
        if (tileID == 14) return TileType::Solid;
        if (tileID == 28) return TileType::Solid;
        if (tileID == 33) return TileType::Solid;

        if (tileID == 18) return TileType::RampLeft;
        if (tileID == 25) return TileType::RampRight;
        return TileType::Empty;
    }

    bool inBounds(int x, int y) const {
        return x >= 0 && x < mapWidth && y >= 0 && y < mapHeight;
    }

private:
    std::vector<std::vector<int> > map;
    std::unordered_set<int> solidTiles = {0, 14, 33, 7, 28, 25};
};
