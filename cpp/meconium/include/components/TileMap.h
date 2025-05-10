#pragma once
#include <SDL.h>
#include <memory>
#include <unordered_set>
#include "components/Component.h"
#include "assets/Definitions.h"

enum class TileType { Empty, Solid, RampLeft, RampRight };

// Tilesheet + map.csv
class TileMap final : public Component {

public:
    TileMap(SDL_Texture* tex, std::shared_ptr<TileSetDefinition> def) : texture(tex), tileSetDefinition(def) {
        // lookup table for tile types
        for (auto it : tileSetDefinition->tiles) {
            tileTypes.insert(it.id, it.type);
        }
    }

    SDL_Texture* texture = nullptr;
    std::shared_ptr<TileSetDefinition> tileSetDefinition;

    int tileSize = 70;
    int mapHeight = 0;      // rows
    int mapWidth = 0;       // cols
    int tilesheetWidth = 0; // Width of tilesheet in pixels

    int at(const int row, const int col) const { return map.at(row).at(col); }

    SDL_Rect getTileSrcRect(int tileID) const;

    bool isSolidTile(int tileID) const { return getTileType(tileID) == TileType::Solid; }

    TileType getTileType(const int tileID) const {
        return tileTypes.at(tileID);
    }

    // bool inBounds(int x, int y) const { return x >= 0 && x < mapWidth && y >= 0 && y < mapHeight; }

private:
    std::vector<std::vector<int>> map;
    std::unordered_map<int, TileType> tileTypes;
};
