#include "systems/TweenSystem.h"
#include "components/Camera.h"
#include "components/Transform.h"
#include "components/Tween.h"

void TweenSystem::update(Entities& entities, const int dt) const {

    for (auto entity : entities.filtered<Tween, Transform>()) {

        auto tween = entity->getComponent<Tween>();
        auto pos = entity->getComponent<Transform>();
        Vec2 endPos = tween->endPos;
        if (tween->relativeEndPos) {
            if (auto camera = entities.findEntityWithComponent<Camera>()) {
                auto camPos = camera->getComponent<Transform>();
                endPos.x += camPos->x;
                endPos.y += camPos->y;
            }
        }

        tween->elapsed += dt;
        float t = tween->elapsed / tween->duration;
        if (t >= 1.0f) {
            t = 1.0f;
            tween->finished = true;
        }

        // Apply easing function (function pointer)
        float easedT = tween->easing ? tween->easing(t) : t;

        // Linear interpolation
        Vec2 newPos = tween->startPos * (1 - easedT) + endPos * easedT;

        // Update entity position component
        pos->x = newPos.x;
        pos->y = newPos.y;

        if (t >= 1.0f) {
            tween->finished = true;
        }
    }
}
