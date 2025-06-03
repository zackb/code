#include "corex/ui/Text.h"
#include "corex/Context.h"
#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <SDL_ttf.h>
#include <iostream>

namespace ui {
    struct Text::Impl {
        const std::string message;
        const Color color;
        const Font& font;
        SDL_Texture* tex = nullptr;
        int w = 0;
        int h = 0;
        Impl(const std::string& msg, const Color& col, const Font& fnt)
       : message(msg), color(col), font(fnt) {}
        ~Impl() {
            if (tex) {
                SDL_DestroyTexture(tex);
            }
        }
    };

    Text::Text(const std::string& message, const Font& font, const Color& color)
        : impl(std::make_unique<Impl>(message, color, font)) {

        SDL_Color sdlColor = {color.r, color.g, color.b, color.a};

        SDL_Surface* surface = TTF_RenderText_Blended(static_cast<TTF_Font*>(font.getSDLFont()), message.c_str(), sdlColor);
        if (!surface) {
            std::cerr << "IMG_Load failed: " << IMG_GetError() << std::endl;
            return;
        }
        impl->tex = SDL_CreateTextureFromSurface(Context::renderer, surface);
        impl->w = surface->w;
        impl->h = surface->h;
        SDL_FreeSurface(surface);
    }

    void Text::draw(int x, int y) const {
        if (!impl->tex)
            return;
        SDL_Rect dst = {x, y, int(impl->w), int(impl->h)};
        SDL_RenderCopy(Context::renderer, impl->tex, nullptr, &dst);
    }

    void Text::draw(int x, int y, const Color& color) const {
        if (color == impl->color) {
            return draw(x, y);
        }

        SDL_Color sdlColor = {color.r, color.g, color.b, color.a};

        SDL_Surface* surface =
            TTF_RenderText_Blended((TTF_Font*)impl->font.getSDLFont(), impl->message.c_str(), sdlColor);
        if (!surface) {
            std::cerr << "IMG_Load failed: " << IMG_GetError() << std::endl;
            return;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(Context::renderer, surface);
        SDL_FreeSurface(surface);
        SDL_Rect dst = {x, y, int(impl->w), int(impl->h)};
        SDL_RenderCopy(Context::renderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }

    int Text::width() const { return impl->w; }
    int Text::height() const { return impl->h; }

    Text::~Text() = default;
    Text::Text(Text&& other) noexcept = default;
    Text& Text::operator=(Text&& other) noexcept = default;
} // namespace ui
