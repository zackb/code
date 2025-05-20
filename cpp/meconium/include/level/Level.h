#pragma once

#include "ResourceManager.h"
#include "assets/AssetLoader.h"
#include "components/Animation.h"
#include "components/Background.h"
#include "components/TileMap.h"
#include "level/Enemy.h"

enum class Facing { LEFT, RIGHT };

using Enemies = std::vector<std::shared_ptr<Enemy>>;

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

    std::shared_ptr<AnimationComponent> createAnimation(const SpriteSheetDefinition& spriteDef) const {

        auto animComponent = std::make_shared<AnimationComponent>();
        for (auto it : spriteDef.animations) {
            auto anim = std::make_shared<Animation>(it.looping);
            for (int i = 0; i < it.frameCount; i++) {
                anim->addFrame(
                    {spriteDef.tileWidth * i, it.row * spriteDef.tileHeight, spriteDef.tileWidth, spriteDef.tileHeight},
                    it.duration);
            }

            AnimationState state = AnimationState::IDLE;
            if (it.name == "idle")
                state = AnimationState::IDLE;
            else if (it.name == "walk")
                state = AnimationState::WALKING;
            else if (it.name == "jump")
                state = AnimationState::JUMPING;
            else if (it.name == "fall")
                state = AnimationState::FALLING;
            else if (it.name == "attack")
                state = AnimationState::ATTACKING;
            else if (it.name == "die")
                state = AnimationState::DYING;
            else
                std::cerr << "Unknown animation state: " << it.name << std::endl;

            animComponent->addAnimation(state, anim);
        }

        animComponent->init();
        return animComponent;
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

    std::string getBackgroundMusic() const { return levelDef->backgroundMusic; }

private:
    std::shared_ptr<LevelDefinition> levelDef;
};
