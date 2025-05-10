#include "Context.h"
#include "ECS.h"
#include "ResourceManager.h"

void RenderSystem::render(const std::vector<std::shared_ptr<Entity>>& entities, TileMap& tileMap) {
    auto camera = findActiveCamera(entities);
    auto camPos = camera->getComponent<Transform>();

    // Render the parallax background layers first
    renderParallaxBackground(*camPos);

    // then the map
    renderTileMap(tileMap, *camPos);

    for (auto entity : entities) {
        auto transform = entity->getComponent<Transform>();
        auto sprite = entity->getComponent<Sprite>();
        if (!transform) {
            continue;
        }

        if (sprite) {

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
        }
    }
}

void RenderSystem::renderTileMap(TileMap& tileMap, Transform& camera) {
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
                            tileMap.tileWidth(),
                            tileMap.tileHeight()};

            SDL_RenderCopy(Context::renderer, tileMap.texture, &src, &dst);
        }
    }
}

void RenderSystem::renderParallaxBackground(Transform& camera) {
    std::vector<ParallaxLayer> layers = {
        {ResourceManager::loadTexture("assets/map_1/background_layer_1.png"), 0.3f},  // Farthest layer
        {ResourceManager::loadTexture("assets/map_1/background_layer_2.png"), 0.5f},  // Middle layer
        {ResourceManager::loadTexture("assets/map_1/background_layer_3.png"), 0.8f},  // Foreground layer
    };

    for (auto& layer : layers) {
        renderLayer(layer, camera);
    }
}

void RenderSystem::renderLayer(const ParallaxLayer& layer, const Transform& camera) {
    int textureW, textureH;
    SDL_QueryTexture(layer.texture, nullptr, nullptr, &textureW, &textureH);

    // Horizontal scroll only
    float scrollX = camera.x * layer.speed;
    int startX = static_cast<int>(scrollX) % Context::windowSize.width;
    if (startX < 0) startX += Context::windowSize.width;

    // Stretch the image vertically to window height and horizontally to one segment width
    int scaledW = Context::windowSize.width;
    int scaledH = Context::windowSize.height;

    for (int x = -startX; x < Context::windowSize.width; x += scaledW) {
        SDL_Rect dst = {x, 0, scaledW, scaledH};
        SDL_RenderCopy(Context::renderer, layer.texture, nullptr, &dst);
    }
}

std::shared_ptr<Entity> RenderSystem::findActiveCamera(const std::vector<std::shared_ptr<Entity>>& entities) const {
    for (const auto& entity : entities) {
        if (entity->hasComponent<Camera>() && entity->hasComponent<Transform>()) {
            return entity;
        }
    }
    std::cerr << "Failed to find camera" << std::endl;
    return nullptr;
}
