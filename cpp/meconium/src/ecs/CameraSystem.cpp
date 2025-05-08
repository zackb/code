#include "ecs/CameraSystem.h"
#include "ecs/ECS.h"

void CameraSystem::update(std::vector<std::shared_ptr<Entity>>& entities, const TileMap& map) const {
    for (auto& e : entities) {
        if (!e->hasComponent<CameraComponent>())
            continue;

        auto camPos = e->getComponent<Position>();
        auto camComp = e->getComponent<CameraComponent>();

        if (e->hasComponent<FollowComponent>()) {
            auto follow = e->getComponent<FollowComponent>();
            if (auto target = follow->target.lock()) {
                auto targetPos = target->getComponent<Position>();

                // Define dead zone size
                int deadZoneWidth = camComp->viewportWidth / 4;
                int deadZoneHeight = camComp->viewportHeight / 3;

                // Calculate dead zone bounds in world space
                int deadZoneLeft = camPos->x + (camComp->viewportWidth - deadZoneWidth) / 2;
                int deadZoneRight = deadZoneLeft + deadZoneWidth;
                int deadZoneTop = camPos->y + (camComp->viewportHeight - deadZoneHeight) / 2;
                int deadZoneBottom = deadZoneTop + deadZoneHeight;

                float targetX = camPos->x;
                float targetY = camPos->y;

                // Only move camera if target is outside dead zone
                if (targetPos->x < deadZoneLeft) {
                    targetX -= (deadZoneLeft - targetPos->x);
                } else if (targetPos->x > deadZoneRight) {
                    targetX += (targetPos->x - deadZoneRight);
                }

                if (targetPos->y < deadZoneTop) {
                    targetY -= (deadZoneTop - targetPos->y);
                } else if (targetPos->y > deadZoneBottom) {
                    targetY += (targetPos->y - deadZoneBottom);
                }

                // Smooth follow (lerp toward adjusted target position)
                camPos->x += (targetX - camPos->x) * follow->lerpFactor;
                camPos->y += (targetY - camPos->y) * follow->lerpFactor;
            }
        }

        // Clamp to map bounds
        camPos->x = std::max(0, std::min(camPos->x, map.mapWidth * map.tileSize - camComp->viewportWidth));
        camPos->y = std::max(0, std::min(camPos->y, map.mapHeight * map.tileSize - camComp->viewportHeight));
    }
}
