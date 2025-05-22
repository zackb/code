#pragma once

#include "GameState.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

class CharacterSelectState : public GameState {
public:
    CharacterSelectState();
    ~CharacterSelectState();

    void handleEvent(SDL_Event& event) override;
    void update() override;
    void render() override;
    std::unique_ptr<GameState> nextState() override;

private:
    struct CharacterOption {
        std::string name;
        SDL_Texture* portrait;
        SDL_Rect rect;
    };

    std::vector<CharacterOption> characters;
    int selected = 0;
    bool startGame = false;
    bool cancel = false;

    TTF_Font* font = nullptr;
    SDL_Color selectedColor = {255, 255, 0, 255};
    SDL_Color normalColor = {200, 200, 200, 255};

    SDL_Texture* renderText(const std::string& text, SDL_Color color);
};
