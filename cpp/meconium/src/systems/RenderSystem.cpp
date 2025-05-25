#include "systems/RenderSystem.h"
#include "Context.h"
#include "components/Animation.h"
#include "components/Background.h"
#include "components/Camera.h"
#include "components/Collider.h"
#include "components/Health.h"
#include "components/Sprite.h"
#include "components/Tag.h"

void RenderSystem::render(Entities& entities, TileMap& tileMap) {

    auto camera = entities.findEntityWithComponent<Camera>();
    auto camPos = camera->getComponent<Transform>();

    // Render the parallax background layers first
    auto background = entities.findEntityWithComponent<ParallaxBackground>();
    renderParallaxBackground(*background, *camPos);

    // then the map
    renderTileMap(tileMap, *camPos);

    for (auto entity : entities.filtered<Transform, Sprite>()) {
        auto transform = entity->getComponent<Transform>();
        auto sprite = entity->getComponent<Sprite>();

        SDL_Rect dstRect;
        dstRect.x = transform->x - camPos->x; // Apply camera offset
        dstRect.y = transform->y - camPos->y; // Apply camera offset
        dstRect.w = static_cast<int>(sprite->width * transform->scaleX);
        dstRect.h = static_cast<int>(sprite->height * transform->scaleY);

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

        // render enemy health bar
        if (entity->hasComponent<EnemyTag>()) {
            auto health = entity->getComponent<Health>();
            auto collider = entity->getComponent<Collider>();
            if (health && collider) {
                if (health->hp < health->max && health->hp > 0) {
                    Rect dst = {dstRect.x + collider->offsetX, dstRect.y, sprite->width, 0};
                    drawHealthBar(dst, 6, health->hp, health->max);
                }
            }
        }
    }

    // render player health bar
    if (auto player = entities.findEntityWithComponent<PlayerTag>()) {
        auto health = player->getComponent<Health>();
        if (health) {
            Rect dst = {20, 30, 200, 0};
            drawHealthBar(dst, 15, health->hp, health->max);
        }
    }
}

void RenderSystem::renderTileMap(const TileMap& tileMap, const Transform& camera) {
    // render only tiles that are on-screen
    int startCol = std::max(0, camera.x / tileMap.tileWidth());
    int startRow = std::max(0, camera.y / tileMap.tileHeight());
    int endCol = std::min(tileMap.mapWidth, (camera.x + Context::windowSize.width) / tileMap.tileWidth() + 1);
    int endRow = std::min(tileMap.mapHeight, (camera.y + Context::windowSize.height) / tileMap.tileHeight() + 1);

    for (int row = startRow; row < endRow; ++row) {
        for (int col = startCol; col < endCol; ++col) {
            int tileID = tileMap.at(row, col);
            if (tileID < 0)
                continue;

            SDL_Rect src = tileMap.getTileSrcRect(tileID);
            SDL_Rect dst = {col * tileMap.tileWidth() - camera.x,
                            row * tileMap.tileHeight() - camera.y,
                            static_cast<int>(tileMap.tileWidth() * tileMap.scale),
                            static_cast<int>(tileMap.tileHeight() * tileMap.scale)};

            SDL_RenderCopy(Context::renderer, tileMap.texture, &src, &dst);
        }
    }
}

void RenderSystem::renderParallaxBackground(Entity& background, const Transform& camera) const {
    auto para = background.getComponent<ParallaxBackground>();
    for (auto& layer : para->getLayers()) {
        renderLayer(layer, camera);
    }
}

void RenderSystem::renderLayer(const Background& layer, const Transform& camera) const {
    int textureW, textureH;
    SDL_QueryTexture(layer.texture, nullptr, nullptr, &textureW, &textureH);

    // Horizontal scroll only
    float scrollX = camera.x * layer.speed;
    int startX = static_cast<int>(scrollX) % Context::windowSize.width;
    if (startX < 0)
        startX += Context::windowSize.width;

    // Stretch the image vertically to window height and horizontally to one segment width
    int scaledW = Context::windowSize.width;
    int scaledH = Context::windowSize.height;

    for (int x = -startX; x < Context::windowSize.width; x += scaledW) {
        SDL_Rect dst = {x, 0, scaledW, scaledH};
        SDL_RenderCopy(Context::renderer, layer.texture, nullptr, &dst);
    }
}

void RenderSystem::drawHealthBar(const Rect& targetRect, int barHeight, int current, int max) const {

    const int barWidth = targetRect.width;
    const int padding = 1;

    float ratio = std::max(0.0f, std::min(1.0f, static_cast<float>(current) / max));
    int fillWidth = static_cast<int>(ratio * (barWidth - 2 * padding));

    SDL_Rect background = {targetRect.x,
                           targetRect.y - barHeight - 2, // draw above the target
                           barWidth,
                           barHeight};

    SDL_Rect fill = {background.x + padding, background.y + padding, fillWidth, barHeight - 2 * padding};

    // black background
    SDL_SetRenderDrawColor(Context::renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(Context::renderer, &background);

    // green -> red depending on health %
    Uint8 red = static_cast<Uint8>((1.0f - ratio) * 255);
    Uint8 green = static_cast<Uint8>(ratio * 255);
    SDL_SetRenderDrawColor(Context::renderer, red, green, 0, 255);
    SDL_RenderFillRect(Context::renderer, &fill);

    // white border
    SDL_SetRenderDrawColor(Context::renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(Context::renderer, &background);
}
