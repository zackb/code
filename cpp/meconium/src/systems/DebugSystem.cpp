#include "systems/DebugSystem.h"

#include "Context.h"
#include "components/Camera.h"
#include "components/Collider.h"
#include "components/Debug.h"
#include "components/InputControl.h"

#include <iostream>

void DebugSystem::update(const std::shared_ptr<Entities>& entities) const {
    auto entity = entities->findEntityWithComponent<Debug>();
    if (!entity) {
        return;
    }
    auto debug = entity->getComponent<Debug>();
    auto input = entity->getComponent<InputControl>();

    // check if we should toggle debugging
    if (input->justPressed(InputKey::DEBUG)) {
        debug->enabled = !debug->enabled;
    }

    if (!debug->enabled) {
        return;
    }

    // find the player
    auto player = entities->findEntityWithComponent<Collider>();
    if (!player) {
        std::cerr << "could not find player" << std::endl;
    }

    // find the tntities and components we're interested in
    auto camera = entities->findEntityWithComponent<Camera>();
    auto collider = player->getComponent<Collider>();
    auto transform = player->getComponent<Transform>();
    auto camPos = camera->getComponent<Transform>();

    // Draw a rect around the player's hit box
    SDL_Rect r = collider->getBounds(transform);
    SDL_Rect hitBox = {r.x - camPos->x, r.y - camPos->y, r.w, r.h};

    SDL_SetRenderDrawColor(Context::renderer, 255, 0, 0, 255); // Red color
    SDL_RenderDrawRect(Context::renderer, &hitBox);

}
