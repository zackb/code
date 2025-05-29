#pragma once

#include "components/TileMap.h"
#include <SDL2/SDL.h>
#include <vector>

struct CameraView {
    int x, y;
    int width, height;
};

class TileMapRenderer {
public:
    TileMapRenderer(SDL_Renderer* renderer, const TileMap& map, int chunkSize);
    ~TileMapRenderer();

    void render(const CameraView& camera);

    const TileMap& map;

private:
    SDL_Renderer* renderer;
    int chunkSize;

    struct Chunk {
        SDL_Texture* texture = nullptr;
    };

    std::vector<std::vector<Chunk>> chunks;

    void generateChunk(int chunkRow, int chunkCol);
};
