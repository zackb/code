#pragma once

#include "corex/GameState.h"
#include "corex/ui/Text.h"
#include "corex/ui/Texture.h"
#include <memory>

class GameOverState final : public GameState {
public:
    GameOverState(Engine& engine);
    ~GameOverState() override;

    void handleEvent() override;
    void update() override;
    void render() override;
    std::unique_ptr<GameState> nextState() override;

private:
    ui::Texture logo;
    ui::Font font;
    ui::Text text;
    bool restart = false;
};
