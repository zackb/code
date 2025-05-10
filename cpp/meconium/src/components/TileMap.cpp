#include "../../include/components/TileMap.h"

#include "ResourceManager.h"

SDL_Rect TileMap::getTileSrcRect(const int tileID) const {
    int tileSize = tileSetDefinition->tileWidth;
    int tilesPerRow = tilesheetWidth / tileSize;
    int row = tileID / tilesPerRow;
    int col = tileID % tilesPerRow;

    SDL_Rect src = {col * tileSize, row * tileSize, tileSize, tileSize};
    return src;
}
