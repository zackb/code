
#include "systems/CameraSystem.h"
#include "components/Camera.h"
#include "components/Transform.h"
#include <algorithm>
#include <cmath>

void CameraSystem::update(Entities& entities, const TileMap& map) const {

    for (auto& e : entities.findByComponents<Camera, Follow>()) {

        auto camPos = e->getComponent<Transform>();
        auto camComp = e->getComponent<Camera>();
        auto follow = e->getComponent<Follow>();

        if (auto target = follow->target.lock()) {
            auto targetPos = target->getComponent<Transform>();

            // Desired camera position: center target in view
            float desiredX = targetPos->x - camComp->viewportWidth / 2.0f;
            float desiredY = targetPos->y - camComp->viewportHeight / 2.0f;

            // Smooth follow (lerp toward desired position)
            camPos->x += (desiredX - camPos->x) * follow->lerpFactor;
            camPos->y += (desiredY - camPos->y) * follow->lerpFactor;

            // Clamp to map bounds (integer math)
            int maxX = std::max(0, map.mapWidth * map.tileWidth() - camComp->viewportWidth);
            int maxY = std::max(0, map.mapHeight * map.tileHeight() - camComp->viewportHeight);

            camPos->x = std::clamp(static_cast<int>(std::round(camPos->x)), 0, maxX);
            camPos->y = std::clamp(static_cast<int>(std::round(camPos->y)), 0, maxY);
        }
    }
}
