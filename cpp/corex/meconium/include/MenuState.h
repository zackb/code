#pragma once

#include "corex/GameState.h"
#include "corex/ui/Font.h"
#include "corex/ui/Text.h"
#include "corex/ui/Texture.h"
#include <string>
#include <vector>

class MenuState : public GameState {
    struct Option {
        std::string name;
        ui::Text text;
    };

public:
    explicit MenuState(Engine& engine);
    ~MenuState() override;

    void handleEvent() override;
    void update() override;
    void render() override;
    std::unique_ptr<GameState> nextState() override;

private:
    ui::Font font;
    ui::Texture logo;
    std::vector<Option> options;
    size_t index = 0;
    bool startGame = false;
    bool quitGame = false;
};
