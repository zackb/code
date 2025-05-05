#pragma once

#include <SDL.h>
#include <iostream>

class ResourceManager
{
public:
    static SDL_Texture *loadTexture(SDL_Renderer *renderer, std::string filePath) noexcept
    {
        if (!renderer) {
            std::cerr << "Renderer is NULL before loadTexture!\n";
        }

        SDL_Surface *surface = IMG_Load(filePath.c_str());
        if (!surface)
        {
            std::cerr << "IMG_Load failed: " << IMG_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture)
        {
            std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        }

        return texture;
    }
};