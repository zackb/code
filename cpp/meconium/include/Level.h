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
        TileSetDefinition tileSetDef = AssetLoader::loadTileSheet(levelDef->tileset);
        auto mapData = AssetLoader::loadMapCSV(levelDef->tilemap);

        SDL_Texture* texture = ResourceManager::loadTexture(tileSetDef.texture);
        TileMap tileMap(texture, std::make_shared<TileSetDefinition>(tileSetDef), mapData);
        tileMap.tileSetDefinition = tileSetDef;
        tileMap.map = mapData;

        return std::make_shared<TileMap>(tileMap);
    }

private:
    std::shared_ptr<LevelDefinition> levelDef;
};