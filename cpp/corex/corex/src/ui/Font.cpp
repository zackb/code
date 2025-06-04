#include "corex/ui/Font.h"
#include <SDL_ttf.h>
#include <iostream>

namespace ui {
    struct Font::Impl {
        TTF_Font* font = nullptr;
        ~Impl() {
            if (font) {
                TTF_CloseFont(font);
            }
        }
    };

    Font::Font(const std::string& path, int size) : impl(std::make_unique<Impl>()) {
        impl->font = TTF_OpenFont(path.c_str(), size);
        if (!impl->font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        }
    }

    Font::~Font() = default;
    Font::Font(Font&&) noexcept = default;
    Font& Font::operator=(Font&&) noexcept = default;

    bool Font::isValid() const { return impl && impl->font; }

    void* Font::getSDLFont() const { return impl->font; }
} // namespace ui
