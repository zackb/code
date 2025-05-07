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

            // Create future position rectangle
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
        // Calculate the tile boundaries to check (slightly expanded to catch edge cases)
        int startX = (rect.x - 1) / tileMap.tileSize;
        int endX = (rect.x + rect.w) / tileMap.tileSize;
        int startY = (rect.y - 1) / tileMap.tileSize;
        int endY = (rect.y + rect.h) / tileMap.tileSize;

        position->onGround = false;

        // First handle ramp collisions - they take priority
        handleAllRampCollisions(position, velocity, size, rect, startX, endX, startY, endY, tileMap);

        // Only check for solid collisions if we're not already on ground from a ramp
        if (!position->onGround) {
            for (int y = startY; y <= endY; ++y)
            {
                for (int x = startX; x <= endX; ++x)
                {
                    if (x < 0 || y < 0 || x >= tileMap.mapWidth || y >= tileMap.mapHeight)
                        continue;

                    int tileID = tileMap.at(y, x);
                    TileType type = tileMap.getTileType(tileID);

                    if (type == TileType::Solid)
                    {
                        SDL_Rect tileRect = {
                            x * tileMap.tileSize,
                            y * tileMap.tileSize,
                            tileMap.tileSize,
                            tileMap.tileSize};

                        SDL_Rect intersection;
                        if (SDL_IntersectRect(&rect, &tileRect, &intersection))
                        {
                            handleSolidCollision(rect, velocity, position, size, tileRect, intersection);
                        }
                    }
                }
            }
        }
    }

    // New method to handle all ramp collisions at once
    void handleAllRampCollisions(std::shared_ptr<Position> &position,
                              std::shared_ptr<Velocity> &velocity,
                              std::shared_ptr<Size> &size,
                              SDL_Rect &rect,
                              int startX, int endX, int startY, int endY,
                              TileMap &tileMap)
    {
        // These will hold the best ramp collision info
        float bestRampY = std::numeric_limits<float>::max();
        bool foundRamp = false;

        // Check all candidate ramp tiles
        for (int y = startY; y <= endY; ++y)
        {
            for (int x = startX; x <= endX; ++x)
            {
                if (x < 0 || y < 0 || x >= tileMap.mapWidth || y >= tileMap.mapHeight)
                    continue;

                int tileID = tileMap.at(y, x);
                TileType type = tileMap.getTileType(tileID);

                if (type == TileType::RampLeft || type == TileType::RampRight)
                {
                    SDL_Rect tileRect = {
                        x * tileMap.tileSize,
                        y * tileMap.tileSize,
                        tileMap.tileSize,
                        tileMap.tileSize};

                    // Find where player would be on this ramp
                    float rampY = calculateRampY(position, velocity, size, type, tileRect, tileMap);

                    // Check if player is within horizontal bounds of this ramp
                    int playerMidX = position->x + size->width / 2;
                    if (playerMidX >= tileRect.x && playerMidX < tileRect.x + tileRect.w) {
                        // If player's feet are below the ramp surface and this is the highest ramp found
                        int playerFeet = position->y + size->height;
                        if (playerFeet > rampY && playerFeet <= tileRect.y + tileRect.h && rampY < bestRampY) {
                            bestRampY = rampY;
                            foundRamp = true;
                        }
                    }
                }
            }
        }

        // Apply the best ramp collision if one was found
        if (foundRamp && velocity->vy >= 0) { // Only snap to ramp when falling or running
            position->y = bestRampY - size->height;
            velocity->vy = 0;
            position->onGround = true;
        }
    }

    float calculateRampY(std::shared_ptr<Position> &position,
                        std::shared_ptr<Velocity> &velocity,
                        std::shared_ptr<Size> &size,
                        TileType rampType,
                        SDL_Rect tileRect,
                        TileMap &tileMap)
    {
        int tileSize = tileMap.tileSize;
        int playerMidX = position->x + size->width / 2;
        int relX = playerMidX - tileRect.x;

        // Ensure relX is within bounds
        relX = std::max(0, std::min(relX, tileSize - 1));

        float rampHeightRatio = 0;
        if (rampType == TileType::RampRight) {
            // For RampLeft, height decreases from left to right
            // At x=0, height=tileSize, at x=tileSize, height=0
            rampHeightRatio = 1.0f - (float)relX / tileSize;
        } else if (rampType == TileType::RampLeft) {
            // For RampRight, height increases from left to right
            // At x=0, height=0, at x=tileSize, height=tileSize
            rampHeightRatio = (float)relX / tileSize;
        }

        return tileRect.y + (1.0f - rampHeightRatio) * tileSize;
    }

    void handleSolidCollision(SDL_Rect &rect,
                              std::shared_ptr<Velocity> &velocity,
                              std::shared_ptr<Position> &position,
                              std::shared_ptr<Size> &size,
                              SDL_Rect tileRect,
                              SDL_Rect &intersection)
    {
        // Use intersection data to determine which side collided

        // First, handle vertical collisions (they typically have priority)
        if (intersection.h <= intersection.w)
        {
            if (rect.y + rect.h - intersection.h <= tileRect.y) {
                // Top collision - player's feet hit ground
                position->y = tileRect.y - size->height;
                velocity->vy = 0;
                position->onGround = true;
            }
            else if (rect.y >= tileRect.y + tileRect.h - intersection.h) {
                // Bottom collision - player's head hit ceiling
                position->y = tileRect.y + tileRect.h;
                velocity->vy = 0;
            }
        }
        // Only handle horizontal collisions if there wasn't a stronger vertical collision
        else if (intersection.w < intersection.h)
        {
            // Only consider significant horizontal overlaps (prevents "teleporting")
            if (intersection.w >= 4) {
                if (rect.x + rect.w - intersection.w <= tileRect.x) {
                    // Right collision
                    position->x = tileRect.x - size->width;
                    velocity->vx = 0;
                }
                else if (rect.x >= tileRect.x + tileRect.w - intersection.w) {
                    // Left collision
                    position->x = tileRect.x + tileRect.w;
                    velocity->vx = 0;
                }
            }
        }
    }
};