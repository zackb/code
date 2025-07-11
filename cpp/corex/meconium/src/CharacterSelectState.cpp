#include "CharacterSelectState.h"
#include "Meconium.h"
#include "MenuState.h"
#include "corex/Context.h"
#include "corex/FileUtils.h"
#include "corex/Utils.h"
#include "corex/ui/Input.h"
#include "corex/ui/Texture.h"
#include <iostream>
#include <memory>

CharacterSelectState::CharacterSelectState(Engine& engine)
    : GameState(engine), font(resolveAssetPath("assets/fonts/OpenSans-VariableFont_wdth,wght.ttf"), 24) {

    std::vector<std::string> characterNames = {"Blue", "Archer", "Xena"};
    for (const auto& name : characterNames) {
        characters.emplace_back(name,
                                ui::Text(name, font, normalColor),
                                ui::Texture(resolveAssetPath("assets/characters/" + util::tolower(name) + ".png")));
    }
}

void CharacterSelectState::handleEvent() {
    using namespace ui;

    // Query keys pressed this frame, handle navigation or selection
    if (Input::keyPressed(Key::Left)) {
        selected = (selected - 1 + characters.size()) % characters.size();
    }
    if (Input::keyPressed(Key::Right)) {
        selected = (selected + 1) % characters.size();
    }
    if (Input::keyPressed(Key::Enter)) {
        startGame = true;
    }
    if (Input::keyPressed(Key::Escape)) {
        cancel = true;
    }
}

void CharacterSelectState::update(int deltaTime) {
    // optional animations
}

void CharacterSelectState::render() {

    ui::Renderer::clear();

    int spacing = 200;
    int startX = (Context::windowSize.width - spacing * characters.size()) / 2;
    int yImage = 200;
    int yText = yImage + 128 + 10;

    for (size_t i = 0; i < characters.size(); ++i) {
        auto& c = characters[i];
        int x = startX + int(i * spacing);

        // Center texture in its spacing slot
        int imgX = x + spacing / 2 - c.texture.width() / 2;
        c.texture.draw(imgX, yImage);

        // Center text below the image
        int textX = x + spacing / 2 - c.text.width() / 2;
        c.text.draw(textX, yText, (i == selected) ? selectedColor : normalColor);
    }

    ui::Renderer::present();
}

std::unique_ptr<GameState> CharacterSelectState::nextState() {

    // check if the enter key has been hit on a selection
    if (startGame) {
        auto& selection = characters[selected];
        auto game = std::make_unique<Meconium>(engine);
        if (game->init(util::tolower(selection.name), "level1")) {
            return game;
        } else {
            std::cerr << "Failed to initialize game state.\n";
        }
    }

    if (cancel) {
        return std::make_unique<MenuState>(engine);
    }

    return nullptr;
}
