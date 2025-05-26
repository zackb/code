#include "systems/UIRenderSystem.h"

#include "Context.h"
#include "Rect.h"
#include "components/Health.h"
#include "components/Tag.h"
#include <SDL_rect.h>

void UIRenderSystem::render(Entities& entities) const {

    auto player = entities.findEntityWithComponent<PlayerTag>();
    renderPlayerHealthBar(*player);
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
