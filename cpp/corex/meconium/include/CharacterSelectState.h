#pragma once

#include "corex/GameState.h"
#include "corex/ui/Text.h"
#include "corex/ui/Texture.h"
#include <string>
#include <vector>

class CharacterSelectState : public GameState {

public:
    CharacterSelectState(Engine& engine);
    ~CharacterSelectState() = default;

    void handleEvent() override;
    void update(int deltaTime) override;
    void render() override;
    std::unique_ptr<GameState> nextState() override;

private:
    struct Character {
        std::string name;
        ui::Text text;
        ui::Texture texture;
    };

    std::vector<Character> characters;
    ui::Font font;
    size_t selected = 0;
    bool startGame = false;
    bool cancel = false;

    ui::Color selectedColor = {255, 255, 0, 255};
    ui::Color normalColor = {200, 200, 200, 255};
};
