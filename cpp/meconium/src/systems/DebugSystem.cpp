#include "systems/DebugSystem.h"

#include "Context.h"
#include "components/Camera.h"
#include "components/Collider.h"
#include "components/Debug.h"
#include "components/InputControl.h"
#include "components/Sprite.h"
#include "components/State.h"
#include "components/TileMap.h"

#include <iostream>

#include "components/Tag.h"

void DebugSystem::update(Entities& entities, TileMap& tileMap) const {

    auto debugEntity = entities.findEntityWithComponent<Debug>();
    if (!debugEntity) {
        return;
    }
    auto debug = debugEntity->getComponent<Debug>();
    auto input = debugEntity->getComponent<InputControl>();

    // check if we should toggle debugging
    if (input->justPressed(InputKey::DEBUG)) {
        debug->enabled = !debug->enabled;
    }

    if (!debug->enabled) {
        // hide cursor
        SDL_ShowCursor(SDL_DISABLE);
        return;
    }

    // show cursor
    SDL_ShowCursor(SDL_ENABLE);

    // camera info
    auto camera = entities.findEntityWithComponent<Camera>();
    auto camPos = camera->getComponent<Transform>();

    // for every entity with a collider, draw a rect around it
    for (auto& entity : entities.filtered<Collider, Transform>()) {
        auto collider = entity->getComponent<Collider>();
        auto transform = entity->getComponent<Transform>();

        // Draw a rect around the entity's collider
        SDL_Rect r = collider->getBounds(transform);
        SDL_Rect hitBox = {r.x - camPos->x, r.y - camPos->y, r.w, r.h};

        SDL_SetRenderDrawColor(Context::renderer, 255, 0, 0, 255); // red color
        SDL_RenderDrawRect(Context::renderer, &hitBox);

        // Draw a rect around the entity sprite
        auto sprite = entity->getComponent<Sprite>();
        auto pos = entity->getComponent<Transform>();

        if (sprite && pos) {
            SDL_Rect box = {pos->x - camPos->x,
                            pos->y - camPos->y,
                            static_cast<int>(sprite->width * pos->scaleY),
                            static_cast<int>(sprite->height * pos->scaleX)};
            SDL_SetRenderDrawColor(Context::renderer, 0, 0, 255, 255); // blue color
            SDL_RenderDrawRect(Context::renderer, &box);
        }

        // Draw a rect around the entity's hitbox
        if (auto hitbox = entity->getComponent<Hitbox>()) {
            SDL_Rect r = hitbox->collider.getBounds(transform);
            SDL_Rect hitBox = {r.x - camPos->x, r.y - camPos->y, r.w, r.h};

            SDL_SetRenderDrawColor(Context::renderer, 255, 165, 0, 255); // orange color
            SDL_RenderDrawRect(Context::renderer, &hitBox);
        }
    }

    // draw rects around each tile of the tilemap
    SDL_SetRenderDrawColor(Context::renderer, 0, 255, 0, 255); // green color
    for (int i = 0; i < tileMap.mapWidth; i++) {
        for (int j = 0; j < tileMap.mapHeight; j++) {
            SDL_Rect rect = {i * tileMap.tileWidth() - camPos->x,
                             j * tileMap.tileHeight() - camPos->y,
                             tileMap.tileWidth(),
                             tileMap.tileHeight()};
            SDL_RenderDrawRect(Context::renderer, &rect);
        }
    }

    // print player position
    auto player = entities.findEntityWithComponent<PlayerTag>();
    if (player) {
        auto playerPos = player->getComponent<Transform>();
        std::cout << "Player position: (" << playerPos->x << ", " << playerPos->y << ")" << std::endl;
        std::cout << "Player state: (" << static_cast<int>(player->getComponent<State>()->currentAction) << ")"
                  << std::endl;
    }
}
