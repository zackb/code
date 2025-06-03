#include "corex/ui/Font.h"
#include <SDL_ttf.h>
#include <iostream>

namespace ui {
    struct Font::Impl {
        TTF_Font* font = nullptr;
    };

    Font::Font(const std::string& path, int size) : impl(new Impl) {
        impl->font = TTF_OpenFont(path.c_str(), size);
        if (!impl->font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        }
    }

    Font::~Font() {
        if (impl->font) {
            TTF_CloseFont(impl->font);
        }
        delete impl;
    }

    bool Font::isValid() const { return impl && impl->font; }

    void* Font::getSDLFont() const { return impl->font; }
} // namespace ui
