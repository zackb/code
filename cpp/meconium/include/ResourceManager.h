#pragma once

#include <SDL.h>
#include <iostream>

#include "Context.h"

class ResourceManager
{
public:
    static std::shared_ptr<Sprite> loadSprite(std::string filePath, int maxWidth, int maxHeight) noexcept
    {
        SDL_Texture *tex = loadTexture(filePath);
        if (!tex)
        {
            std::cerr << "failed to load texture for sprite" << std::endl;
            return nullptr;
        }

        auto sprite = std::make_shared<Sprite>();
        sprite->texture = tex;
        SDL_QueryTexture(tex, nullptr, nullptr, &sprite->width, &sprite->height);

        // Get original image dimensions
        int originalWidth = sprite->width;
        int originalHeight = sprite->height;

        // Calculate scale factor to fit within the bounds
        float scaleX = static_cast<float>(maxWidth) / originalWidth;
        float scaleY = static_cast<float>(maxHeight) / originalHeight;

        // Use the smaller scale factor to ensure the image fits within both dimensions
        float scaleFactor = std::min(scaleX, scaleY);

        // Calculate new width and height based on the scale factor
        sprite->width = static_cast<int>(originalWidth * scaleFactor);
        sprite->height = static_cast<int>(originalHeight * scaleFactor);

        return sprite;
    }

    static SDL_Texture *loadTexture(std::string filePath) noexcept
    {
        if (!Context::renderer) {
            std::cerr << "Renderer is NULL before loadTexture!\n";
        }

        SDL_Surface *surface = IMG_Load(filePath.c_str());
        if (!surface)
        {
            std::cerr << "IMG_Load failed: " << IMG_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(Context::renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture)
        {
            std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        }

        return texture;
    }
};