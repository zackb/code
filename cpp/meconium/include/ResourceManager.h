#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

#include "Animation.h"
#include "Context.h"
#include "ecs/Sprite.h"

class ResourceManager {

private:
    static std::unordered_map<std::string, SDL_Texture *> textures;

public:
    static std::shared_ptr<Sprite> loadSprite(std::string filePath, int maxWidth, int maxHeight);

    static SDL_Texture* loadTexture(std::string filePath);

    static std::shared_ptr<Animation> createAnimation(const std::string &name, bool looping = true) {
        return std::make_shared<Animation>(name, looping);
    }

    static std::shared_ptr<AnimationComponent> createPlayerAnimations(std::string &spriteSheetPath, int frameWidth,
                                                                      int frameHeight);
    static void cleanup();
};
