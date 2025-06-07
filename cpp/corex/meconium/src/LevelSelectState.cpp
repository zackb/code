#include "LevelSelectState.h"
#include "Meconium.h"
#include "corex/Context.h"
#include "corex/FileUtils.h"
#include "corex/ui/Input.h"
#include "corex/ui/Renderer.h"
#include <iostream>

LevelSelectState::LevelSelectState(Engine& engine, const std::string& currentLevel, const std::string& currentPlayer)
    : GameState(engine)
    , currentLevel(currentLevel)
    , currentPlayer(currentPlayer)
    , font(resolveAssetPath("assets/fonts/OpenSans-VariableFont_wdth,wght.ttf"), 24)
    , text("Congratulations!", font, {144, 144, 0, 255}) {}

void LevelSelectState::handleEvent() {
    if (ui::Input::keyPressed(ui::Key::Enter)) {
        startGame = true;
    }
}
void LevelSelectState::update(int deltaTime) { totalTime += deltaTime; }

void LevelSelectState::render() {

    ui::Renderer::clear();

    text.draw((Context::windowSize.width - text.width()) / 2, (Context::windowSize.height - text.height()) / 2);

    ui::Renderer::present();
}

std::unique_ptr<GameState> LevelSelectState::nextState() {

    // check if the enter key has been hit on a selection
    if (startGame) {
        auto game = std::make_unique<Meconium>(engine);
        // get the current level and increment the number
        std::string prefix;
        int level = 0;
        size_t i = 0;

        while (i < currentLevel.size() && !std::isdigit(currentLevel[i])) {
            i++;
        }

        prefix = currentLevel.substr(0, i);
        level = std::stoi(currentLevel.substr(i));
        level++;

        if (game->init(currentPlayer, prefix + std::to_string(level))) {
            return game;
        } else {
            std::cerr << "Failed to initialize game state.\n";
        }
    }

    return nullptr;
}
