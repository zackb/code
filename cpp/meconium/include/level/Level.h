#pragma once

#include "Pickup.h"
#include "ResourceManager.h"
#include "assets/AssetLoader.h"
#include "components/Background.h"
#include "components/TileMap.h"
#include "level/Enemy.h"

enum class Facing { LEFT, RIGHT };

class Level {
public:
    explicit Level(const std::string& filePath) { levelDef = AssetLoader::loadLevel(filePath); }

    // TODO: move to EntityFactory
    // build the TileMap component
    std::shared_ptr<TileMap> createTileMap() const {
        auto tileSetDef = AssetLoader::loadTileSet(levelDef->tileset);
        auto mapData = AssetLoader::loadMapCSV(levelDef->tilemap);

        SDL_Texture* texture = ResourceManager::loadTexture("assets/" + tileSetDef->texture);
        TileMap tileMap(texture, tileSetDef, mapData);

        return std::make_shared<TileMap>(tileMap);
    }

    std::shared_ptr<ParallaxBackground> createBackground() const {
        std::vector<Background> backgrounds;
        for (int i = 0; i < levelDef->background.layers.size(); i++) {
            auto layer = levelDef->background.layers[i];
            auto texture = ResourceManager::loadTexture("assets/" + layer.texture);
            backgrounds.emplace_back(texture, layer.speed, i);
        }
        return std::make_shared<ParallaxBackground>(backgrounds);
    }

    Enemies createEnemies() const {
        Enemies enemies;
        for (auto e : levelDef->enemies) {
            auto spriteSheet = AssetLoader::loadSpriteSheet(e.sprite);
            Enemy enemy{e, spriteSheet};
            enemies.push_back(std::make_shared<Enemy>(enemy));
        }
        return enemies;
    }

    Pickups createPickups() const {
        Pickups pickups;
        for (auto p : levelDef->pickups) {
            Pickup::Type type;
            if (p.type == "health")
                type = Pickup::Type::HEALTH;
            else if (p.type == "key")
                type = Pickup::Type::KEY;
            auto spriteSheet = AssetLoader::loadSpriteSheet(p.sprite);
            pickups.emplace_back(std::make_shared<Pickup>(p, type, spriteSheet));
        }
        return pickups;
    }

    std::string getBackgroundMusic() const { return levelDef->backgroundMusic; }

private:
    std::shared_ptr<LevelDefinition> levelDef;
};
