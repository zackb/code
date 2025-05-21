#pragma once
#include "Definitions.h"
#include "FileUtils.h"
#include "json.hpp"
#include "level/Player.h"

#include <fstream>
#include <iostream>
#include <string>

class AssetLoader {
public:
    static std::vector<std::vector<int>> loadMapCSV(std::string filePath);

    static std::shared_ptr<Player> loadPlayer(std::string filePath) {
        auto player = loadJson<PlayerDefinition>(filePath);
        auto spriteDef = loadSpriteSheet(player->sprite);
        return std::make_shared<Player>(*player, spriteDef, player->attack);
    }

    static std::shared_ptr<SpriteSheetDefinition> loadSpriteSheet(std::string filePath) {
        return loadJson<SpriteSheetDefinition>(filePath);
    }

    static std::shared_ptr<TileSetDefinition> loadTileSet(std::string filePath) {
        return loadJson<TileSetDefinition>(filePath);
    }

    static std::shared_ptr<MapDefinition> loadMap(std::string filePath) { return loadJson<MapDefinition>(filePath); }
    static std::shared_ptr<LevelDefinition> loadLevel(std::string filePath) {
        return loadJson<LevelDefinition>(filePath);
    }

    template <typename T> static std::shared_ptr<T> loadJson(const std::string& path) {
        std::ifstream file(resolveAssetPath(path));
        if (!file) {
            throw std::runtime_error("Failed to open file: " + path);
        }
        nlohmann::json j;
        file >> j;
        try {
            return std::make_shared<T>(j.get<T>());
        } catch (const std::exception& e) {
            std::cerr << "Error parsing JSON for type " << typeid(T).name() << ": " << e.what() << std::endl;
            throw;
        }
    }
};