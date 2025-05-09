#include "Context.h"
#include "ecs/ECS.h"

void RenderSystem::render(const std::vector<std::shared_ptr<Entity>>& entities, TileMap& tileMap) {
    auto camera = findActiveCamera(entities);
    auto camPos = camera->getComponent<Position>();

    renderTileMap(tileMap, *camPos);

    for (auto entity : entities) {
        auto position = entity->getComponent<Position>();
        auto sprite = entity->getComponent<Sprite>();
        if (!position) {
            continue;
        }

        if (sprite) {
            auto& pos = *position;

            SDL_Rect dstRect;
            dstRect.x = pos.x - camPos->x; // Apply camera offset
            dstRect.y = pos.y - camPos->y; // Apply camera offset
            dstRect.w = static_cast<int>(sprite->width * sprite->scale);
            dstRect.h = static_cast<int>(sprite->height * sprite->scale);

            SDL_Rect srcRect;
            SDL_Rect* srcRectPtr = nullptr;

            // Check if entity has an animation component
            auto animation = entity->getComponent<AnimationComponent>();
            if (animation) {
                srcRect = animation->getCurrentFrame();
                srcRectPtr = &srcRect;
            }

            // Apply flip if needed
            SDL_RendererFlip flip = SDL_FLIP_NONE;
            if (sprite->flipX)
                flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
            if (sprite->flipY)
                flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);

            SDL_RenderCopyEx(Context::renderer, sprite->texture, srcRectPtr, &dstRect, 0, nullptr, flip);
        }
    }
}

void RenderSystem::renderTileMap(TileMap& tileMap, Position& camera) {
    // render only tiles that are on-screen
    int startCol = std::max(0, camera.x / tileMap.tileSize);
    int startRow = std::max(0, camera.y / tileMap.tileSize);
    int endCol = std::min(tileMap.mapWidth, (camera.x + Context::windowSize.width) / tileMap.tileSize + 1);
    int endRow = std::min(tileMap.mapHeight, (camera.y + Context::windowSize.height) / tileMap.tileSize + 1);

    for (int row = startRow; row < endRow; ++row) {
        for (int col = startCol; col < endCol; ++col) {
            int tileID = tileMap.at(row, col);
            if (tileID < 0)
                continue;

            SDL_Rect src = tileMap.getTileSrcRect(tileID);
            SDL_Rect dst = {col * tileMap.tileSize - camera.x,
                            row * tileMap.tileSize - camera.y,
                            tileMap.tileSize,
                            tileMap.tileSize};

            SDL_RenderCopy(Context::renderer, tileMap.texture, &src, &dst);
        }
    }
}

std::shared_ptr<Entity> RenderSystem::findActiveCamera(const std::vector<std::shared_ptr<Entity>>& entities) const {
    for (const auto& entity : entities) {
        if (entity->hasComponent<CameraComponent>() &&
            entity->hasComponent<Position>()) {
            return entity;
            }
    }
    std::cerr << "Failed to find camera" << std::endl;
    return nullptr;
}
