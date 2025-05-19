#include "MenuState.h"
#include "Context.h"
#include <iostream>
#include "FileUtils.h"
#include "Meconium.h"
#include <SDL_image.h>

#include "GameOverState.h"


MenuState::MenuState() {
    font = TTF_OpenFont(resolveAssetPath("assets/fonts/OpenSans-VariableFont_wdth,wght.ttf").c_str(), 36);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    const std::string logoPath = resolveAssetPath("assets/images/logo.png");
    SDL_Surface* logoSurface = IMG_Load(logoPath.c_str());
    if (!logoSurface) {
        std::cerr << "Failed to load logo: " << IMG_GetError() << std::endl;
    } else {
        logoTexture = SDL_CreateTextureFromSurface(Context::renderer, logoSurface);
        SDL_FreeSurface(logoSurface);

        if (logoTexture) {
            SDL_QueryTexture(logoTexture, nullptr, nullptr, &logoRect.w, &logoRect.h);
            // Scale it to 50%
            float scale = 0.5f;
            logoRect.w = static_cast<int>(logoRect.w * scale);
            logoRect.h = static_cast<int>(logoRect.h * scale);
            // Center horizontally, position near top
            logoRect.x = (Context::windowSize.width - logoRect.w) / 2;
        }
    }
}

MenuState::~MenuState() {
    if (font) TTF_CloseFont(font);
    if (logoTexture) SDL_DestroyTexture(logoTexture);
}

void MenuState::handleEvent(SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        quitGame = true;
    }

    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                index = (index - 1 + options.size()) % options.size();
                break;
            case SDLK_DOWN:
                index = (index + 1) % options.size();
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                if (options[index] == "Start Game") {
                    startGame = true;
                } else if (options[index] == "Exit") {
                    quitGame = true;
                } else if (options[index] == "Options") {
                    // Handle "Options"
                }
                break;
            default:
                std::cout << "Unknown Event: " << event.key.keysym.sym << std::endl;
        }
    }
}

void MenuState::update() {
    // Add animation or somthing
}

void MenuState::render() {
    SDL_SetRenderDrawColor(Context::renderer, 0, 0, 0, 255);
    SDL_RenderClear(Context::renderer);

    if (logoTexture) {
        SDL_RenderCopy(Context::renderer, logoTexture, nullptr, &logoRect);
    }

    int y = logoRect.h;
    for (size_t i = 0; i < options.size(); ++i) {
        SDL_Color color = (i == index) ? selectedColor : normalColor;
        SDL_Texture* tex = renderText(options[i], color);

        if (tex) {
            int w, h;
            SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
            SDL_Rect dst = {Context::windowSize.width / 2 - w / 2, y, w, h};
            SDL_RenderCopy(Context::renderer, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
        }

        y += 60;
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

std::unique_ptr<GameState> MenuState::nextState() {
    if (startGame) {
        auto game = std::make_unique<Meconium>();
        if (game->init()) {
            return game;
        } else {
            std::cerr << "Failed to initialize game state.\n";
        }
    }

    if (quitGame) {
        // return std::make_unique<GameOverState>();
        exit(0); // TODO: CLEAN
    }

    return nullptr;
}
