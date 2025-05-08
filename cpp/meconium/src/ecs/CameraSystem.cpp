#include "ecs/CameraSystem.h"
#include "ecs/ECS.h"

#include "ecs/CameraSystem.h"
#include "ecs/ECS.h"
#include <algorithm>
#include <cmath>

void CameraSystem::update(std::vector<std::shared_ptr<Entity>>& entities, const TileMap& map) const {
    for (auto& e : entities) {
        if (!e->hasComponent<CameraComponent>() || !e->hasComponent<FollowComponent>())
            continue;

        auto camPos = e->getComponent<Position>();
        auto camComp = e->getComponent<CameraComponent>();
        auto follow = e->getComponent<FollowComponent>();

        if (auto target = follow->target.lock()) {
            auto targetPos = target->getComponent<Position>();

            // Desired camera position: center target in view
            float desiredX = targetPos->x - camComp->viewportWidth / 2.0f;
            float desiredY = targetPos->y - camComp->viewportHeight / 2.0f;

            // Smooth follow (lerp toward desired position)
            camPos->x += (desiredX - camPos->x) * follow->lerpFactor;
            camPos->y += (desiredY - camPos->y) * follow->lerpFactor;

            // Clamp to map bounds (integer math)
            int maxX = std::max(0, map.mapWidth * map.tileSize - camComp->viewportWidth);
            int maxY = std::max(0, map.mapHeight * map.tileSize - camComp->viewportHeight);

            camPos->x = std::clamp(static_cast<int>(std::round(camPos->x)), 0, maxX);
            camPos->y = std::clamp(static_cast<int>(std::round(camPos->y)), 0, maxY);
        }
    }
}
