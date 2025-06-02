#include <SDL.h>
#include <memory>
#include <unistd.h>

#include "Context.h"
#include "Meconium.h"

#include "Engine.h"
#include "GameOverState.h"
#include "assets/AssetLoader.h"
#include "components/Bag.h"
#include "components/Camera.h"
#include "components/Health.h"
#include "components/State.h"
#include "components/Tag.h"
#include "entity/EntityFactory.h"

bool Meconium::init(std::string character) {

    // Load player definition
    auto playerDef = AssetLoader::loadPlayer("assets/players/" + character + ".json");

    engine.loadLevel("level2");

    // Create a player entity
    auto player = engine.createEntity();

    // Tag as player entity
    player->addComponent<PlayerTag>();

    // Give player some health
    player->addComponent<Health>(100);

    // Add Sprite
    auto sprite = EntityFactory::createSprite(*playerDef->spriteDef);
    player->addComponent<Sprite>(sprite);

    // Add animation component
    auto animComponent = EntityFactory::createAnimation(*playerDef->spriteDef);
    player->addComponent<AnimationComponent>(animComponent);

    // Add attack
    player->addComponent<Attack>(EntityFactory::createAttack(playerDef->attack));

    // Add velocity
    player->addComponent<Velocity>(0, 0);

    // Add input control to the player
    player->addComponent<InputControl>();

    // Add Collision box
    if (playerDef->spriteDef->collider.has_value()) {
        auto rect = playerDef->spriteDef->collider.value();
        player->addComponent<Collider>(rect.x, rect.y, rect.width, rect.height);
    } else {
        player->addComponent<Collider>(0, 0, sprite->width, sprite->height);
    }

    // Add hitbox if we should
    if (playerDef->hitbox.has_value()) {
        auto rect = playerDef->hitbox.value();
        player->addComponent<Hitbox>(rect.x, rect.y, rect.width, rect.height);
    }

    // Add Transform
    player->addComponent<Transform>(0, 0, playerDef->spriteDef->scale);

    // Add State
    player->addComponent<State>();

    // Add a bag to the player
    player->addComponent<Bag>();

    // add camera
    auto camera = engine.createEntity();
    camera->addComponent<Transform>(0, 0);
    camera->addComponent<Camera>(Context::windowSize.width, Context::windowSize.height);
    camera->addComponent<Follow>(player, 0.2f); // smooth follow

    engine.enableDebug();

    return true;
}

void Meconium::update() {

    // Check for player death
    if (!engine.entities().findEntityWithComponent<PlayerTag>()) {
        next = std::make_unique<GameOverState>(engine);
    }
}

void Meconium::render() {}

void Meconium::handleEvent(SDL_Event& event) {
    // TODO: do we need this?
}

std::unique_ptr<GameState> Meconium::nextState() {
    return std::move(next); // Return ownership of next state if set
}
