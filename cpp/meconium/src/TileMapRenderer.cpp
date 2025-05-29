#include "TileMapRenderer.h"

TileMapRenderer::TileMapRenderer(const TileMap& m, SDL_Renderer* r, int chunkSz)
    : map(m), renderer(r), chunkSize(chunkSz) {

    // make a big enough vector of vectors to hold all the chunks
    int rows = (map.mapHeight + chunkSize - 1) / chunkSize;
    int cols = (map.mapWidth + chunkSize - 1) / chunkSize;
    chunks.resize(rows, std::vector<Chunk>(cols));

    // create chunkSz x chunkSz grid of textures
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            generateChunk(row, col);
        }
    }
}

TileMapRenderer::~TileMapRenderer() {
    for (auto& row : chunks) {
        for (auto& chunk : row) {
            SDL_DestroyTexture(chunk.texture);
        }
    }
}

// Combine multiple tiles into chunks for fewer render calls
void TileMapRenderer::generateChunk(int chunkRow, int chunkCol) {

    const int texW = chunkSize * map.tileWidth();
    const int texH = chunkSize * map.tileHeight();
    const int tileWidth = map.tileWidth();
    const int tileHeight = map.tileHeight();

    SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, texW, texH);

    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, tex);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    for (int row = 0; row < chunkSize; ++row) {
        for (int col = 0; col < chunkSize; ++col) {
            int mapRow = chunkRow * chunkSize + row;
            int mapCol = chunkCol * chunkSize + col;

            if (mapRow >= map.mapHeight || mapCol >= map.mapWidth)
                continue;

            int tileID = map.at(mapRow, mapCol);
            if (tileID < 0) {
                continue;
            }

            SDL_Rect src = map.getTileSrcRect(tileID);
            SDL_Rect dst = {col * tileWidth, row * tileHeight, tileWidth, tileHeight};
            SDL_RenderCopy(renderer, map.texture, &src, &dst);
        }
    }

    SDL_SetRenderTarget(renderer, nullptr);
    chunks[chunkRow][chunkCol].texture = tex;
}

// render chunks of tiles
void TileMapRenderer::render(const CameraView& cam) {
    const int chunkPixelSize = chunkSize * map.tileWidth();

    int firstCol = cam.x / chunkPixelSize;
    int firstRow = cam.y / chunkPixelSize;
    int lastCol = (cam.x + cam.width) / chunkPixelSize;
    int lastRow = (cam.y + cam.height) / chunkPixelSize;

    for (int row = firstRow; row <= lastRow; ++row) {
        for (int col = firstCol; col <= lastCol; ++col) {
            if (row < 0 || col < 0 || row >= (int)chunks.size() || col >= (int)chunks[0].size())
                continue;

            Chunk& chunk = chunks[row][col];
            if (!chunk.texture)
                continue;

            SDL_Rect dst = {col * chunkPixelSize - cam.x, row * chunkPixelSize - cam.y, chunkPixelSize, chunkPixelSize};

            SDL_RenderCopy(renderer, chunk.texture, nullptr, &dst);
        }
    }
}
