#pragma once

#include "TileMap.h"

class CameraSystem {
    public:
        void update(std::shared_ptr<Entity> entity, std::shared_ptr<CameraComponent> camera, TileMap& tileMap) {
            auto& pos = entity->getComponent<Position>();
            // Logic to move the camera based on entity position
            if (pos->x < camera->x + camera->bufferZoneX) {
                camera->x = std::max(0, pos->x - camera->bufferZoneX);
            } else if (pos->x + pos->width > camera->x + camera->width - camera->bufferZoneX) {
                camera->x = std::min(tileMap.mapWidth * tileMap.tileSize - camera->width, pos->x + pos->width - camera->width + camera->bufferZoneX);
            }
    
            if (pos->y < camera->y + camera->bufferZoneY) {
                camera->y = std::max(0, pos->y - camera->bufferZoneY);
            } else if (pos->y + pos->height > camera->y + camera->height - camera->bufferZoneY) {
                camera->y = std::min(tileMap.mapHeight * tileMap.tileSize - camera->height, pos->y + pos->height - camera->height + camera->bufferZoneY);
            }
        }
    };