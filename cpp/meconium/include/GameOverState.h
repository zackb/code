#pragma once

#include "GameState.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <memory>

class GameOverState final : public GameState {
public:
    GameOverState();
    ~GameOverState() override;

    void handleEvent(SDL_Event& event) override;
    void update() override;
    void render() override;
    std::unique_ptr<GameState> nextState() override;

private:
    TTF_Font* font;
    SDL_Texture* textTexture;
    SDL_Rect textRect;
    bool restart = false;

    SDL_Texture* renderText(const std::string& message, SDL_Color color);
};
