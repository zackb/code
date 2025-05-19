#include "MenuState.h"
#include "Context.h"
#include <iostream>
#include "FileUtils.h"

MenuState::MenuState() {
    font = TTF_OpenFont(resolveAssetPath("assets/fonts/OpenSans-VariableFont_wdth,wght.ttf").c_str(), 36);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    constexpr SDL_Color white = {255, 255, 255, 255};
    textTexture = renderText("Press Enter to Start", white);

    if (textTexture) {
        SDL_QueryTexture(textTexture, nullptr, nullptr, &textRect.w, &textRect.h);
        textRect.x = (Context::windowSize.width - textRect.w) / 2;
        textRect.y = (Context::windowSize.height - textRect.h) / 2;
    }
}

MenuState::~MenuState() {
    if (textTexture) SDL_DestroyTexture(textTexture);
    if (font) TTF_CloseFont(font);
}

void MenuState::handleEvent(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
        startGame = true;
    }
}

void MenuState::update() {
    // No-op for now
}

void MenuState::render() {
    SDL_SetRenderDrawColor(Context::renderer, 20, 20, 30, 255);
    SDL_RenderClear(Context::renderer);

    if (textTexture) {
        SDL_RenderCopy(Context::renderer, textTexture, nullptr, &textRect);
    }

    SDL_RenderPresent(Context::renderer);
}

SDL_Texture* MenuState::renderText(const std::string& message, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, message.c_str(), color);
    if (!surface) {
        std::cerr << "TTF_RenderText failed: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(Context::renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
