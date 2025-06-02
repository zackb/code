#include "corex/ResourceManager.h"

#include "corex/Context.h"
#include "corex/FileUtils.h"

#include <SDL_image.h>
#include <iostream>

std::unordered_map<std::string, SDL_Texture*> ResourceManager::textures;

SDL_Texture* ResourceManager::loadTexture(const std::string& filePath) {
    std::string path = resolveAssetPath(filePath);
    // Check if we already loaded this texture
    if (const auto it = textures.find(path); it != textures.end()) {
        return it->second;
    }

    if (!Context::renderer) {
        std::cerr << "Renderer is NULL before loadTexture!\n";
    }

    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "IMG_Load failed: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(Context::renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
    }
    // Cache the texture
    textures[path] = texture;

    return texture;
}

void ResourceManager::cleanup() {
    for (auto& pair : textures) {
        SDL_DestroyTexture(pair.second);
    }
    textures.clear();
}
