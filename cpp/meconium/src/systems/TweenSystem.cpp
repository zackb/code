#include "systems/TweenSystem.h"
#include "components/Transform.h"
#include "components/Tween.h"

void TweenSystem::update(Entities& entities, const int dt) const {

    for (auto entity : entities.filtered<Tween, Transform>()) {

        auto tween = entity->getComponent<Tween>();
        auto pos = entity->getComponent<Transform>();

        tween->elapsed += dt;
        float t = std::min(tween->elapsed / tween->duration, 1.0f);

        // Linear interpolation
        Vec2 newPos = tween->startPos * (1 - t) + tween->endPos * t;

        // Update entity position component
        pos->x = newPos.x;
        pos->y = newPos.y;

        if (t >= 1.0f) {
            tween->finished = true;
        }
    }
}
