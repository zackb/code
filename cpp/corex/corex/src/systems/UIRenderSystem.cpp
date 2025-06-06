#include "corex/systems/UIRenderSystem.h"

#include "corex/Context.h"
#include "corex/FileUtils.h"
#include "corex/components/Health.h"
#include "corex/components/LevelComponent.h"
#include "corex/components/Tag.h"
#include "corex/ui/Text.h"
#include <SDL_rect.h>
#include <memory>

void UIRenderSystem::render(Entities& entities) {

    if (auto player = entities.findEntityWithComponent<PlayerTag>()) {

        renderPlayerHealthBar(*player);

        if (auto bag = player->getComponent<Bag>()) {
            renderBag(*bag);
        }

        renderMessages(entities);
    }
}
void UIRenderSystem::renderMessages(Entities& entities) {

    // one time initialize font. We cant do this in constructor because SDL isnt ready
    if (!font) {
        font = std::make_unique<ui::Font>(resolveAssetPath("assets/fonts/OpenSans-VariableFont_wdth,wght.ttf"), 36);
    }

    for (auto entity : entities.filtered<MissingKey>()) {
        auto message = entity->getComponent<MissingKey>();
        ui::Text text(message->message(), *font, {0, 0, 0, 255});
        text.draw(400, 400);
    }
}

void UIRenderSystem::renderBag(Bag& bag) const {
    const int kStartX = 20;
    const int kStartY = 30;
    const int kItemSize = 30;
    const int kItemSpacing = 2;
    const int padding = 4;
    const int itemBoxWidth = (kItemSize + kItemSpacing) * static_cast<int>(bag.items.size()) - kItemSpacing;
    const int boxWidth = itemBoxWidth + 2 * padding;
    const int boxHeight = kItemSize + (2 * padding);

    SDL_Rect bagRect = {kStartX, kStartY, boxWidth, boxHeight};

    /*
    // draw dark background
    SDL_SetRenderDrawColor(Context::renderer, 0, 0, 0, 220); // semi-transparent black
    SDL_RenderDrawRect(Context::renderer, &bagRect);
    */

    // draw each item
    int x = bagRect.x;
    int y = bagRect.y + padding;

    for (auto& item : bag.items) {

        SDL_Rect srcRect = {0, 0, item.sprite->width, item.sprite->height};
        if (item.animation) {
            srcRect = item.animation->getCurrentFrame();
        }

        // Apply flip if needed
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (item.sprite->flipX)
            flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
        if (item.sprite->flipY)
            flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);

        SDL_Rect dstRect = {x, y, kItemSize, kItemSize};
        SDL_RenderCopyEx(Context::renderer, item.sprite->texture, &srcRect, &dstRect, 0, nullptr, flip);

        x += kItemSize + kItemSpacing + padding;
    }
}

void UIRenderSystem::renderPlayerHealthBar(Entity& player) const {
    auto health = player.getComponent<Health>();
    if (!health) {
        std::cerr << "no health component on player\n";
        return;
    }
    Rect dst = {20, 30, 200, 0};
    renderHealthBar(dst, 15, health->hp, health->max);
}

void UIRenderSystem::renderHealthBar(const Rect& targetRect, int barHeight, int current, int max) {

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
