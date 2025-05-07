#pragma once

class CollisionSystem
{
public:
    void update(std::vector<std::shared_ptr<Entity>> &entities, TileMap &tileMap)
    {
        for (auto &entity : entities)
        {
            auto position = entity->getComponent<Position>();
            auto velocity = entity->getComponent<Velocity>();
            auto size = entity->getComponent<Size>();

            if (!position || !velocity || !size)
                continue;

            // Save original position
            int originalX = position->x;
            int originalY = position->y;

            SDL_Rect futureRect = {
                position->x + velocity->vx,
                position->y + velocity->vy,
                size->width,
                size->height};

            resolveTileCollisions(futureRect, velocity, position, size, tileMap);

            // If no collisions modified position, apply velocity as normal
            if (position->x == originalX && position->y == originalY) {
                position->x += velocity->vx;
                position->y += velocity->vy;
            }

            // fell off the world
            if (position->y > tileMap.mapHeight * tileMap.tileSize * 2) {
                std::cout << "fell off" << std::endl;
                position->x = 100; // respawn X
                position->y = 100; // respawn Y
                velocity->vx = 0;
                velocity->vy = 0;
                position->onGround = false;
            }
        }
    }

private:
    void resolveTileCollisions(SDL_Rect &rect,
                               std::shared_ptr<Velocity> &velocity,
                               std::shared_ptr<Position> &position,
                               std::shared_ptr<Size> &size,
                               TileMap &tileMap)
    {
        int startX = rect.x / tileMap.tileSize;
        int endX = (rect.x + rect.w - 1) / tileMap.tileSize;
        int startY = rect.y / tileMap.tileSize;
        int endY = (rect.y + rect.h - 1) / tileMap.tileSize;

        position->onGround = false;

        for (int y = startY; y <= endY; ++y)
        {
            for (int x = startX; x <= endX; ++x)
            {
                if (x < 0 || y < 0 || x >= tileMap.mapWidth || y >= tileMap.mapHeight)
                    continue;

                int tileID = tileMap.at(y, x);
                TileType type = tileMap.getTileType(tileID);

                SDL_Rect tileRect = {
                    x * tileMap.tileSize,
                    y * tileMap.tileSize,
                    tileMap.tileSize,
                    tileMap.tileSize};

                SDL_Rect intersection;
                if (!SDL_IntersectRect(&rect, &tileRect, &intersection))
                    continue;

                switch (type)
                {
                case TileType::Solid:
                    handleSolidCollision(rect, velocity, position, size, tileRect);
                    break;

                case TileType::RampLeft:
                case TileType::RampRight:
                    handleRampCollision(position, velocity, size, type, tileRect, tileMap);
                    break;

                default:
                    break;
                }
            }
        }
    }

    void handleSolidCollision(SDL_Rect &rect,
                              std::shared_ptr<Velocity> &velocity,
                              std::shared_ptr<Position> &position,
                              std::shared_ptr<Size> &size,
                              SDL_Rect tileRect)
    {
        SDL_Rect intersection;
        if (!SDL_IntersectRect(&rect, &tileRect, &intersection))
            return;

        // Vertical collision
        if (intersection.w >= intersection.h)
        {
            if (velocity->vy > 0)
            { // Falling
                position->y = tileRect.y - size->height;
                velocity->vy = 0;
                position->onGround = true;
            }
            else if (velocity->vy < 0)
            { // Jumping
              // Don't force position->y unless there's an actual ceiling
                // If you always "resolve upward", you cancel jumping
                // Make sure this only applies if we hit the ceiling
                if (position->y < tileRect.y + tileRect.h)
                {
                    position->y = tileRect.y + tileRect.h;
                    velocity->vy = 0;
                }
            }
        }
        else // Horizontal collision
        {
            if (velocity->vx > 0)
            { // Moving right
                if (intersection.w >= 4)
                    position->x = tileRect.x - size->width;
            }
            else if (velocity->vx < 0)
            { // Moving left
                if (intersection.w >= 4)
                    position->x = tileRect.x + tileRect.w;
            }
            velocity->vx = 0;
        }
    }

    void handleRampCollision(std::shared_ptr<Position> &position,
                             std::shared_ptr<Velocity> &velocity,
                             std::shared_ptr<Size> &size,
                             TileType rampType,
                             SDL_Rect tileRect,
                             TileMap &tileMap)
    {
        int tileSize = tileMap.tileSize;
        int playerMidX = position->x + size->width / 2;

        int relX = playerMidX - tileRect.x;

        if (relX < 0 || relX >= tileSize)
            return; // Not inside this ramp

        int rampHeight = 0;
        if (rampType == TileType::RampLeft)
        {
            rampHeight = tileSize - relX; // rising left to right
        }
        else if (rampType == TileType::RampRight)
        {
            rampHeight = relX; // rising right to left
        }

        int rampTopY = tileRect.y + rampHeight - size->height;
        int playerFeet = position->y + size->height;
        int rampY = tileRect.y + rampHeight;

        if (playerFeet > rampY /*- 5 */ && playerFeet <= tileRect.y + tileSize)
        {
            // Snap to ramp surface
            position->y = rampTopY;
            velocity->vy = 0;
            position->onGround = true;
        }
    }
};
    