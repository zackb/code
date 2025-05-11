#include "systems/DebugSystem.h"

#include "Context.h"
#include "components/Camera.h"
#include "components/Collider.h"
#include "components/Debug.h"
#include <iostream>

void DebugSystem::update(const std::shared_ptr<Entities>& entities) const {
    auto debug = entities->findFirstComponent<Debug>();
    if (!debug || !debug->enabled) {
        return;
    }
    auto player = entities->findEntityWithComponent<Collider>();
    if (!player) {
        std::cerr << "could not find player" << std::endl;
    }

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
