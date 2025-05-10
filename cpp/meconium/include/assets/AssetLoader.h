#pragma once
#include "Definitions.h"
#include "FileUtils.h"
#include "json.hpp"

#include <string>
#include <fstream>

class AssetLoader {
public:
    static std::vector<std::vector<int>> loadMapCSV(std::string filePath);

    std::shared_ptr<SpriteSheetDefinition> loadSpriteSheet(std::string filePath) {
        return loadJson<SpriteSheetDefinition>(filePath);
    }

    std::shared_ptr<TileSetDefinition> loadTileSet(std::string filePath) {
        return loadJson<TileSetDefinition>(filePath);
    }

    std::shared_ptr<MapDefinition> loadMap(std::string filePath) {
        return loadJson<MapDefinition>(filePath);
    }
    std::shared_ptr<LevelDefinition> loadLevel(std::string filePath) {
        return loadJson<LevelDefinition>(filePath);
    }

    template <typename T> static std::shared_ptr<T> loadJson(const std::string& path) {
        std::ifstream file(resolveAssetPath(path));
        if (!file) {
            throw std::runtime_error("Failed to open file: " + path);
        }
        nlohmann::json j;
        file >> j;
        return std::make_shared<T>(j.get<T>());
    }
};