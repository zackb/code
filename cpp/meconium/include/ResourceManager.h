#pragma once

#include <SDL.h>

class ResourceManager
{
    public:
        static SDL_Texture* loadTexture(SDL_Renderer* renderer, std::string filePath) noexcept {
            SDL_Surface* surface = IMG_Load(filePath.c_str());
            if (!surface) return nullptr;
        
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
            return texture;
        }
};