#pragma once
#include "ResourceManager.h"
#include "assets/AssetLoader.h"

class Level {
public:
    Level(std::string filePath) {
        levelDef = AssetLoader::loadLevel(filePath);
    }

    // build the TileMap component
    std::shared_ptr<TileMap> createTileMap() {
        auto tileSetDef = AssetLoader::loadTileSet(levelDef->tileset);
        auto mapData = AssetLoader::loadMapCSV(levelDef->tilemap);

        SDL_Texture* texture = ResourceManager::loadTexture("assets/" + tileSetDef->texture);
        TileMap tileMap(texture, tileSetDef, mapData);

        return std::make_shared<TileMap>(tileMap);
    }

private:
    std::shared_ptr<LevelDefinition> levelDef;
};