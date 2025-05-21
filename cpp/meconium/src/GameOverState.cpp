#include "GameOverState.h"
#include "Context.h"
#include "MenuState.h"
#include <SDL_image.h>
#include <iostream>

#include "FileUtils.h"

GameOverState::GameOverState() {
    const std::string logoPath = resolveAssetPath("assets/images/gameover.png");
    SDL_Surface* logoSurface = IMG_Load(logoPath.c_str());
    if (!logoSurface) {
        std::cerr << "Failed to load game over: " << IMG_GetError() << std::endl;
    } else {
        logoTexture = SDL_CreateTextureFromSurface(Context::renderer, logoSurface);
        SDL_FreeSurface(logoSurface);

        if (logoTexture) {
            SDL_QueryTexture(logoTexture, nullptr, nullptr, &logoRect.w, &logoRect.h);
            float scale = 0.5f;
            logoRect.w = static_cast<int>(logoRect.w * scale);
            logoRect.h = static_cast<int>(logoRect.h * scale);
            // center
            logoRect.x = (Context::windowSize.width - logoRect.w) / 2;
            logoRect.y = (Context::windowSize.height - logoRect.h) / 2;
        }
    }

    font = TTF_OpenFont(resolveAssetPath("assets/fonts/OpenSans-VariableFont_wdth,wght.ttf").c_str(), 36);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color red = {255, 0, 0, 255};
    textTexture = renderText("Press Enter to Retry", red);

    if (textTexture) {
        SDL_QueryTexture(textTexture, nullptr, nullptr, &textRect.w, &textRect.h);
        textRect.x = (Context::windowSize.width - textRect.w) / 2;
        textRect.y = logoRect.y + logoRect.h;
    }
}

GameOverState::~GameOverState() {
    if (logoTexture)
        SDL_DestroyTexture(logoTexture);
    if (textTexture)
        SDL_DestroyTexture(textTexture);
    if (font)
        TTF_CloseFont(font);
}

void GameOverState::handleEvent(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
        restart = true;
    }
}

void GameOverState::update() {
    // Could animate or time fade-in, etc.
}

void GameOverState::render() {
    SDL_SetRenderDrawColor(Context::renderer, 0, 0, 0, 255);
    SDL_RenderClear(Context::renderer);

    if (logoTexture) {
        SDL_RenderCopy(Context::renderer, logoTexture, nullptr, &logoRect);
    }

    if (textTexture) {
        SDL_RenderCopy(Context::renderer, textTexture, nullptr, &textRect);
    }

    SDL_RenderPresent(Context::renderer);
}

std::unique_ptr<GameState> GameOverState::nextState() {
    if (restart) {
        return std::make_unique<MenuState>();
    }
    return nullptr;
}

SDL_Texture* GameOverState::renderText(const std::string& message, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, message.c_str(), color);
    if (!surface) {
        std::cerr << "TTF_RenderText failed: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(Context::renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
