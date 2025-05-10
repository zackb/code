#pragma once

#include <SDL.h>
#include <iostream>

#include "components/Animation.h"
#include "components/Sprite.h"

class ResourceManager {

private:
    static std::unordered_map<std::string, SDL_Texture*> textures;

public:
    static std::shared_ptr<Sprite> loadSprite(const std::string& path, int frameWidth, int frameHeight);
    static SDL_Texture* loadTexture(const std::string& filePath);

    static void cleanup();
};
