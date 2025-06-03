#include "MenuState.h"
#include "CharacterSelectState.h"
#include "MenuState.h"
#include "corex/Context.h"
#include "corex/FileUtils.h"
#include "corex/ui/Font.h"
#include "corex/ui/Input.h"
#include "corex/ui/Renderer.h"
#include "corex/ui/Texture.h"
#include <iostream>

MenuState::MenuState(Engine& engine)
    : GameState(engine)
    , font(resolveAssetPath("assets/fonts/OpenSans-VariableFont_wdth,wght.ttf"), 36)
    , logo(resolveAssetPath("assets/images/logo.png")) {
    if (!font.isValid()) {
        std::cerr << "Failed to load font." << std::endl;
    }
    if (!logo.isValid()) {
        std::cerr << "Failed to load logo." << std::endl;
    }

    for (auto name : {"Start Game", "Options", "Exit"}) {
        options.emplace_back(name, ui::Text(name, font, {255, 255, 255, 255}));
    }
}

MenuState::~MenuState() = default;

void MenuState::handleEvent() {
    if (ui::Input::keyPressed(ui::Key::Up)) {
        index = (index - 1 + options.size()) % options.size();
    } else if (ui::Input::keyPressed(ui::Key::Down)) {
        index = (index + 1) % options.size();
    } else if (ui::Input::keyPressed(ui::Key::Enter) || ui::Input::keyPressed(ui::Key::Space)) {
        if (options[index].name == "Start Game") {
            startGame = true;
        } else if (options[index].name == "Exit") {
            quitGame = true;
        } else if (options[index].name == "Options") {
            // Handle Options
        }
    }
}

void MenuState::update() {
    // Add animation or something
}

void MenuState::render() {
    ui::Renderer::clear();

    if (logo.isValid()) {
        int x = (Context::windowSize.width - logo.width() / 2) / 2;
        logo.draw(x, 50, 0.5f);
    }

    int y = (logo.height() / 2) + 60;
    for (size_t i = 0; i < options.size(); ++i) {
        ui::Color color = (i == index) ? ui::Color{255, 255, 0, 255} : ui::Color{255, 255, 255, 255};
        int x = (Context::windowSize.width - options[i].text.width()) / 2;
        options[i].text.draw(x, y, color);
        y += 60;
    }

    ui::Renderer::present();
}

std::unique_ptr<GameState> MenuState::nextState() {
    if (startGame) {
        return std::make_unique<CharacterSelectState>(engine);
    }
    if (quitGame) {
        exit(0); // TODO: CLEAN
    }
    return nullptr;
}
