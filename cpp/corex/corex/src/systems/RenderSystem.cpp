#include "corex/systems/RenderSystem.h"
#include "corex/Context.h"
#include "corex/components/Animation.h"
#include "corex/components/Background.h"
#include "corex/components/Camera.h"
#include "corex/components/Collider.h"
#include "corex/components/Health.h"
#include "corex/components/Sprite.h"
#include "corex/components/Tag.h"
#include "corex/systems/UIRenderSystem.h"

void RenderSystem::render(Entities& entities, TileMapRenderer& tileMapRenderer) {

    auto camera = entities.findEntityWithComponent<Camera>();
    auto camPos = camera->getComponent<Transform>();
    auto camSize = camera->getComponent<Camera>();

    // Render the parallax background layers first
    auto background = entities.findEntityWithComponent<ParallaxBackground>();
    renderParallaxBackground(*background, *camPos);

    // then the map
    // TODO: Is this chunking really worth it?
    //       it turns out SDL batches internally on the same texture!
    // renderTileMap(tileMapRenderer.map, *camPos);
    tileMapRenderer.render({
        .x = camPos->x,
        .y = camPos->y,
        .width = camSize->viewportWidth,
        .height = camSize->viewportHeight,
    });

    for (auto& entity : entities.filtered<Transform, Sprite>()) {
        auto transform = entity->getComponent<Transform>();
        auto sprite = entity->getComponent<Sprite>();

        // Don't render entities outside of the camera
        if (transform->x + (sprite->width * transform->scaleX) < camPos->x ||
            transform->x > camPos->x + camSize->viewportWidth) {
            continue;
        }

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
                    // reuse health bar drawing logic from the UI
                    UIRenderSystem::renderHealthBar(dst, 6, health->hp, health->max);
                }
            }
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
