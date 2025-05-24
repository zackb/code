#include "CharacterSelectState.h"
#include "Context.h"
#include "FileUtils.h"
#include "Meconium.h"
#include "MenuState.h"
#include "Utils.h"
#include <SDL_image.h>
#include <iostream>
#include <memory>

CharacterSelectState::CharacterSelectState() {
    font = TTF_OpenFont(resolveAssetPath("assets/fonts/OpenSans-VariableFont_wdth,wght.ttf").c_str(), 24);

    std::vector<std::string> characterNames = {"Blue", "Archer", "Xena"};
    for (const auto& name : characterNames) {
        CharacterOption c;
        c.name = name;
        std::string imgPath = resolveAssetPath("assets/characters/" + util::tolower(name) + ".png");
        SDL_Surface* surface = IMG_Load(imgPath.c_str());
        c.portrait = surface ? SDL_CreateTextureFromSurface(Context::renderer, surface) : nullptr;
        SDL_FreeSurface(surface);
        characters.push_back(c);
    }
}

CharacterSelectState::~CharacterSelectState() {
    for (auto& c : characters) {
        if (c.portrait)
            SDL_DestroyTexture(c.portrait);
    }
    if (font)
        TTF_CloseFont(font);
}

void CharacterSelectState::handleEvent(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_LEFT:
            selected = (selected - 1 + characters.size()) % characters.size();
            break;
        case SDLK_RIGHT:
            selected = (selected + 1) % characters.size();
            break;
        case SDLK_RETURN:
            startGame = true;
            break;
        case SDLK_ESCAPE:
            cancel = true;
            break;
        }
    }
}

void CharacterSelectState::update() {
    // optional animations
}

void CharacterSelectState::render() {
    SDL_SetRenderDrawColor(Context::renderer, 0, 0, 0, 255);
    SDL_RenderClear(Context::renderer);

    int spacing = 200;
    int startX = (Context::windowSize.width - spacing * characters.size()) / 2;

    for (size_t i = 0; i < characters.size(); ++i) {
        auto& c = characters[i];
        SDL_Rect dst = {startX + int(i * spacing), 200, 128, 128};
        c.rect = dst;

        if (c.portrait) {
            SDL_RenderCopy(Context::renderer, c.portrait, nullptr, &dst);
        }

        SDL_Color color = (i == selected) ? selectedColor : normalColor;
        SDL_Texture* nameTex = renderText(c.name, color);
        if (nameTex) {
            int w, h;
            SDL_QueryTexture(nameTex, nullptr, nullptr, &w, &h);
            SDL_Rect nameRect = {dst.x + dst.w / 2 - w / 2, dst.y + dst.h + 10, w, h};
            SDL_RenderCopy(Context::renderer, nameTex, nullptr, &nameRect);
            SDL_DestroyTexture(nameTex);
        }
    }

    SDL_RenderPresent(Context::renderer);
}

std::unique_ptr<GameState> CharacterSelectState::nextState() {

    // check if the enter key has been hit on a selection
    if (startGame) {
        auto selection = characters[selected];
        auto game = std::make_unique<Meconium>();
        if (game->init(util::tolower(selection.name))) {
            return game;
        } else {
            std::cerr << "Failed to initialize game state.\n";
        }
    }

    if (cancel) {
        return std::make_unique<MenuState>();
    }

    return nullptr;
}

SDL_Texture* CharacterSelectState::renderText(const std::string& text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface)
        return nullptr;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(Context::renderer, surface);
    SDL_FreeSurface(surface);
    return tex;
}
