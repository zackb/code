#pragma once

#include <SDL.h>
#include <string>
#include <unordered_map>

class ResourceManager {

private:
    static std::unordered_map<std::string, SDL_Texture*> textures;

public:
    static SDL_Texture* loadTexture(const std::string& filePath);

    static void cleanup();
};
