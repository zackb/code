#pragma once

#include "GameState.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <memory>

class MenuState final : public GameState {
public:
    MenuState();
    ~MenuState() override;

    void handleEvent(SDL_Event& event) override;
    void update() override;
    void render() override;
    std::unique_ptr<GameState> nextState() override;

private:
    TTF_Font* font = nullptr;

    std::vector<std::string> options = {"Start Game", "Options", "Exit"};
    size_t index = 0;

    SDL_Color normalColor = {255, 255, 255, 255};
    SDL_Color selectedColor = {255, 255, 0, 255};

    bool startGame = false;
    bool quitGame = false;

    SDL_Texture* renderText(const std::string& message, SDL_Color color);
};