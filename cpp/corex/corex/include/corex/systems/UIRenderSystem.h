#pragma once

#include "corex/Rect.h"
#include "corex/components/Bag.h"
#include "corex/entity/Entity.h"
#include "corex/ui/Font.h"
#include <memory>

class UIRenderSystem {

public:
    void render(Entities& entities);
    static void renderHealthBar(const Rect& targetRect, int barHeight, int current, int max);

private:
    std::unique_ptr<ui::Font> font;
    void renderPlayerHealthBar(Entity& player) const;
    void renderBag(Bag& bag) const;
    void renderMessages(Entities& entities);
};
