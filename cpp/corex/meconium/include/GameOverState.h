#pragma once

#include "corex/GameState.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <string>

class GameOverState final : public GameState {
public:
    GameOverState(Engine& engine);
    ~GameOverState() override;

    void handleEvent() override;
    void update() override;
    void render() override;
    std::unique_ptr<GameState> nextState() override;

private:
    TTF_Font* font;
    SDL_Texture* logoTexture = nullptr;
    SDL_Rect logoRect{};
    SDL_Texture* textTexture;
    SDL_Rect textRect;
    bool restart = false;

    SDL_Texture* renderText(const std::string& message, SDL_Color color);
};
