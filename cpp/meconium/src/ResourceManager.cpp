#include "ResourceManager.h"

#include "Context.h"
#include <SDL_image.h>

std::unordered_map<std::string, SDL_Texture*> ResourceManager::textures;

SDL_Texture* ResourceManager::loadTexture(const std::string& filePath) {
    // Check if we already loaded this texture
    if (const auto it = textures.find(filePath); it != textures.end()) {
        return it->second;
    }

    if (!Context::renderer) {
        std::cerr << "Renderer is NULL before loadTexture!\n";
    }

    SDL_Surface* surface = IMG_Load(filePath.c_str());
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
    textures[filePath] = texture;

    return texture;
}

void ResourceManager::cleanup() {
    for (auto& pair : textures) {
        SDL_DestroyTexture(pair.second);
    }
    textures.clear();
}
