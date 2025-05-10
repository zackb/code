#pragma once
#include "ResourceManager.h"
#include "assets/AssetLoader.h"
#include "components/Background.h"

class Level {
public:
    explicit Level(const std::string& filePath) { levelDef = AssetLoader::loadLevel(filePath); }

    // TODO: move to impl file
    std::shared_ptr<Sprite> createSprite(const std::string& playerPath) const {
        return createSprite(AssetLoader::loadSpriteSheet(playerPath));
    }

    std::shared_ptr<Sprite> createSprite(const std::shared_ptr<SpriteSheetDefinition> spriteDef) const {
        Sprite sprite;
        // TODO: where to resolveAssetPath (loadJson, loadTexture, create*)?
        sprite.texture = ResourceManager::loadTexture("assets/" + spriteDef->texture);
        sprite.height = spriteDef->tileHeight;
        sprite.width = spriteDef->tileWidth;
        return std::make_shared<Sprite>(sprite);
    }

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
            else
                std::cerr << "Unknown animation state: " << it.name << std::endl;

            animComponent->addAnimation(state, anim);
        }

        return animComponent;
    }

    std::shared_ptr<ParallaxBackground> createBackground() const {
        std::vector<std::shared_ptr<Background>> backgrounds;
        for (int i = 0; i < levelDef->background.layers.size(); i++) {
            auto layer = levelDef->background.layers[i];
            auto texture = ResourceManager::loadTexture("assets/" + layer.texture);
            Background background(texture, layer.speed, i);
            backgrounds.push_back(std::make_shared<Background>(background));
        }
        return std::make_shared<ParallaxBackground>(backgrounds);
    }

private:
    std::shared_ptr<LevelDefinition> levelDef;
};