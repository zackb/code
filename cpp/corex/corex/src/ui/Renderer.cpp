#include "corex/ui/Renderer.h"
#include "corex/Context.h"
#include <SDL.h>
#include <SDL_ttf.h>

namespace ui {
    void Renderer::clear(Color color) {
        SDL_SetRenderDrawColor(Context::renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(Context::renderer);
    }

    void Renderer::present() { SDL_RenderPresent(Context::renderer); }

    void Renderer::drawText(const std::string& message, const Font& font, int x, int y, Color color) {
        if (!font.isValid())
            return;

        SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
        SDL_Surface* surface = TTF_RenderText_Blended((TTF_Font*)font.getSDLFont(), message.c_str(), sdlColor);
        if (!surface)
            return;

        SDL_Texture* tex = SDL_CreateTextureFromSurface(Context::renderer, surface);
        SDL_Rect dst = {x, y, surface->w, surface->h};
        SDL_RenderCopy(Context::renderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surface);
    }
} // namespace ui
