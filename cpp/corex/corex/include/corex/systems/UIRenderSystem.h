#pragma once

#include "corex/Rect.h"
#include "corex/components/Bag.h"
#include "corex/entity/Entity.h"

class UIRenderSystem {

public:
    void render(Entities& entities) const;
    static void renderHealthBar(const Rect& targetRect, int barHeight, int current, int max);

private:
    void renderPlayerHealthBar(Entity& player) const;
    void renderBag(Bag& bag) const;
};
