#include "assets/AssetLoader.h"

#include <sstream>

std::shared_ptr<SpriteSheetDefinition> AssetLoader::loadSpriteSheet(std::string filePath) {
    return loadJson<SpriteSheetDefinition>(filePath);
}

std::shared_ptr<TileSetDefinition> AssetLoader::loadTileSet(std::string filePath) {
    return loadJson<TileSetDefinition>(filePath);
}

std::shared_ptr<MapDefinition> AssetLoader::loadMap(std::string filePath) {
    return loadJson<MapDefinition>(filePath);
}
std::shared_ptr<LevelDefinition> AssetLoader::loadLevel(std::string filePath) {
    return loadJson<LevelDefinition>(filePath);
}

std::vector<std::vector<int>> AssetLoader::loadMapCSV(std::string filePath) {
    std::vector<std::vector<int>> map;
    std::string realPath = resolveAssetPath(filePath);

    if (!std::filesystem::exists(realPath)) {
        std::cerr << "Failed to load map from file: " + realPath << std::endl;
    }

    std::ifstream file(realPath);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string val;
        std::vector<int> rowVec;

        while (std::getline(ss, val, ',')) {
            rowVec.push_back(std::stoi(val));
        }

        map.push_back(std::move(rowVec));
    }
    return map;
}
