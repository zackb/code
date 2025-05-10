#pragma once
#include "assets/AssetLoader.h"

class Level {
public:
    Level(std::string filePath) {
        levelDef = AssetLoader::loadLevel(filePath);
    }

    // build the TileMap component
    std::shared_ptr<TileMap> createTileMap() {
        TileSheetDefinition tileSheetDef = AssetLoader::loadTileSheet(levelDef->tileset);

        TileMap::load(levelDef->tilemap, tileSheetDef);
    }

private:
    std::shared_ptr<LevelDefinition> levelDef;
};