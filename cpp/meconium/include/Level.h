#pragma once
#include "ResourceManager.h"
#include "assets/AssetLoader.h"

class Level {
public:
    Level(std::string filePath) { levelDef = AssetLoader::loadLevel(filePath); }

    // build the TileMap component
    std::shared_ptr<TileMap> createTileMap() {
        auto tileSetDef = AssetLoader::loadTileSet(levelDef->tileset);
        auto mapData = AssetLoader::loadMapCSV(levelDef->tilemap);

        SDL_Texture* texture = ResourceManager::loadTexture("assets/" + tileSetDef->texture);
        TileMap tileMap(texture, tileSetDef, mapData);

        return std::make_shared<TileMap>(tileMap);
    }

    static std::shared_ptr<AnimationComponent> createAnimation(const SpriteSheetDefinition& spriteDef) {

        auto animComponent = std::make_shared<AnimationComponent>();
        for (auto it : spriteDef.animations) {
            auto anim = std::make_shared<Animation>(it.looping);
            for (int i = 0; i < it.frameCount; i++) {
                anim->addFrame({spriteDef.tileWidth * i, it.row * spriteDef.tileHeight, spriteDef.tileWidth, spriteDef.tileHeight},
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

private:
    std::shared_ptr<LevelDefinition> levelDef;
};