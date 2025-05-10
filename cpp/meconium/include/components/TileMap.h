#pragma once
#include "assets/Definitions.h"
#include "components/Component.h"
#include <SDL.h>
#include <memory>
#include <unordered_set>

// Tilesheet + map.csv
class TileMap final : public Component {

public:
    SDL_Texture* texture = nullptr;

    int mapHeight = 0;      // rows
    int mapWidth = 0;       // cols
    int tilesheetWidth = 0; // Width of tilesheet in pixels

    TileMap(SDL_Texture* tex, std::shared_ptr<TileSetDefinition> def, std::vector<std::vector<int>> mapData)
        : texture(tex), tileSetDefinition(def), map(mapData) {
        // lookup table for tile types
        for (auto it : tileSetDefinition->tiles) {
            tileTypes[it.id] = it.type;
        }
        mapHeight = mapData.size();
        mapWidth = mapData[0].size();

        // query map to get the size of the image
        int w = 0, h = 0;
        SDL_QueryTexture(tex, NULL, NULL, &w, &h);

        tilesheetWidth = w;
    }

    int at(const int row, const int col) const { return map.at(row).at(col); }

    int tileHeight() const { return tileSetDefinition->tileHeight; }
    int tileWidth() const { return tileSetDefinition->tileWidth; }

    SDL_Rect getTileSrcRect(int tileID) const;

    bool isSolidTile(int tileID) const { return getTileType(tileID) == TileType::Solid; }

    TileType getTileType(const int tileID) const { return tileTypes.at(tileID); }

    // bool inBounds(int x, int y) const { return x >= 0 && x < mapWidth && y >= 0 && y < mapHeight; }

private:
    std::shared_ptr<TileSetDefinition> tileSetDefinition;
    std::vector<std::vector<int>> map;
    std::unordered_map<int, TileType> tileTypes;
};
