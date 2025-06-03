#include "corex/ui/Text.h"
#include "corex/Context.h"
#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <iostream>

namespace ui {
    struct Text::Impl {
        SDL_Texture* tex = nullptr;
        int w = 0;
        int h = 0;
    };

    Text::Text(const std::string& message, const Font& font, const Color& color) : impl(new Impl) {

        SDL_Color sdlColor = {color.r, color.g, color.b, color.a};

        SDL_Surface* surface = TTF_RenderText_Blended((TTF_Font*)font.getSDLFont(), message.c_str(), sdlColor);
        if (!surface) {
            std::cerr << "IMG_Load failed: " << IMG_GetError() << std::endl;
            return;
        }
        impl->tex = SDL_CreateTextureFromSurface(Context::renderer, surface);
        impl->w = surface->w;
        impl->h = surface->h;
        SDL_FreeSurface(surface);
    }
    Text::~Text() {
        if (impl->tex)
            SDL_DestroyTexture(impl->tex);
        delete impl;
    }

    void Text::draw(int x, int y) const {
        if (!impl->tex)
            return;
        SDL_Rect dst = {x, y, int(impl->w), int(impl->h)};
        SDL_RenderCopy(Context::renderer, impl->tex, nullptr, &dst);
    }

    int Text::width() const { return impl->w; }
    int Text::height() const { return impl->h; }
} // namespace ui
