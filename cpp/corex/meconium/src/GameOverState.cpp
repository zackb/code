#include "GameOverState.h"
#include "MenuState.h"
#include <iostream>

#include "corex/Context.h"
#include "corex/FileUtils.h"
#include "corex/ui/Input.h"
#include "corex/ui/Renderer.h"

GameOverState::GameOverState(Engine& engine)
    : GameState(engine)
    , logo(resolveAssetPath("assets/images/gameover.png"))
    , font(resolveAssetPath("assets/fonts/OpenSans-VariableFont_wdth,wght.ttf"), 36)
    , text("Press Enter to Retry", font, {255, 0, 0, 255}) {

    if (!logo.isValid()) {
        std::cerr << "Failed to load game over logo: " << std::endl;
    }
}

GameOverState::~GameOverState() {}

void GameOverState::handleEvent() {
    if (ui::Input::keyPressed(ui::Key::Enter)) {
        restart = true;
    }
}

void GameOverState::update(int deltaTime) {
    // Could animate or time fade-in, etc.
}

void GameOverState::render() {

    ui::Renderer::clear();

    float scale = 0.5f;
    int scaledLogoW = static_cast<int>(logo.width() * scale);
    int scaledLogoH = static_cast<int>(logo.height() * scale);
    int logoX = (Context::windowSize.width - scaledLogoW) / 2;
    int logoY = (Context::windowSize.height - scaledLogoH) / 2;

    logo.draw(logoX, logoY, scale);

    int textX = (Context::windowSize.width - text.width()) / 2;
    int textY = logoY + scaledLogoH + 20;

    text.draw(textX, textY);

    ui::Renderer::present();
}

std::unique_ptr<GameState> GameOverState::nextState() {
    if (restart) {
        return std::make_unique<MenuState>(engine);
    }
    return nullptr;
}
