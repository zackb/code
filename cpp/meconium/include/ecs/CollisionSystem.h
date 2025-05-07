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
        int startX = (rect.x - 2) / tileMap.tileSize;
        int endX = (rect.x + rect.w + 2) / tileMap.tileSize;
        int startY = (rect.y - 2) / tileMap.tileSize;
        int endY = (rect.y + rect.h + 2) / tileMap.tileSize;

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

        // Check all candidate ramp tiles with a wider range
        // Expand the search slightly to catch adjacent ramps
        startX = std::max(0, startX - 1);
        endX = std::min(tileMap.mapWidth - 1, endX + 1);

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

                    // Use a more generous horizontal bounds check for ramps
                    // This helps catch the case when moving between adjacent ramps
                    int playerLeft = position->x;
                    int playerRight = position->x + size->width;
                    bool horizontalOverlap = (playerRight > tileRect.x && playerLeft < tileRect.x + tileRect.w);

                    if (horizontalOverlap) {
                        // If player's feet are below the ramp surface and this is the highest ramp found
                        int playerFeet = position->y + size->height;
                        if (playerFeet > rampY && playerFeet <= tileRect.y + tileRect.h + 5 && rampY < bestRampY) {
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

        float rampHeight = 0;
        if (rampType == TileType::RampRight) {
            rampHeight = tileSize - relX;
        } else if (rampType == TileType::RampLeft) {
            rampHeight = relX;
        }

        return tileRect.y + tileSize - rampHeight;
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