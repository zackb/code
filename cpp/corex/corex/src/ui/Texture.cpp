#include "corex/ui/Texture.h"
#include "corex/Context.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

namespace ui {
    struct Texture::Impl {
        SDL_Texture* tex = nullptr;
        int w = 0;
        int h = 0;
    };

    Texture::Texture(const std::string& path) : impl(new Impl) {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            std::cerr << "IMG_Load failed: " << IMG_GetError() << std::endl;
            return;
        }
        impl->tex = SDL_CreateTextureFromSurface(Context::renderer, surface);
        impl->w = surface->w;
        impl->h = surface->h;
        SDL_FreeSurface(surface);
    }

    Texture::~Texture() {
        if (impl->tex)
            SDL_DestroyTexture(impl->tex);
        delete impl;
    }

    bool Texture::isValid() const { return impl && impl->tex; }

    int Texture::width() const { return impl->w; }
    int Texture::height() const { return impl->h; }

    void Texture::draw(int x, int y, float scale) const {
        if (!impl->tex)
            return;
        SDL_Rect dst = {x, y, int(impl->w * scale), int(impl->h * scale)};
        SDL_RenderCopy(Context::renderer, impl->tex, nullptr, &dst);
    }
} // namespace ui
