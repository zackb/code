#pragma once
#include "GameState.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class MenuState : public GameState {
public:
    MenuState();
    ~MenuState() override;

    void handleEvent(SDL_Event& event) override;
    void update() override;
    void render() override;
    bool shouldStartGame() const { return startGame; }

private:
    TTF_Font* font;
    SDL_Texture* textTexture;
    SDL_Rect textRect;
    bool startGame = false;

    SDL_Texture* renderText(const std::string& message, SDL_Color color);
};
