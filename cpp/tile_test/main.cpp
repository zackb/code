// NOTE: This is a simplified ECS-like example with SDL2
// Assumes: tilesheet.png and map.csv exist, with tile size 70x70

#include <SDL.h>
#include <SDL_image.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 70;
const int MAP_WIDTH = 89;
const int MAP_HEIGHT = 20;
int tilesheetCols = 0;


struct Camera {
    int x, y, width, height;
} camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

struct TransformComponent {
    int x, y, width, height;
};

struct SpriteComponent {
    SDL_Texture* texture;
    SDL_Rect srcRect;
};

// Mock Entity
struct Entity {
    TransformComponent transform;
    SpriteComponent sprite;
};

int tileMap[MAP_HEIGHT][MAP_WIDTH];

SDL_Rect getTileSrcRect(int tileID) {
    SDL_Rect src;
    src.x = (tileID % tilesheetCols) * TILE_SIZE;
    src.y = (tileID / tilesheetCols) * TILE_SIZE;
    src.w = src.h = TILE_SIZE;
    return src;
}

void loadMap(const std::string& path) {
    std::ifstream file(path);
    std::string line;
    int row = 0;

    while (std::getline(file, line) && row < MAP_HEIGHT) {
        std::stringstream ss(line);
        std::string val;
        int col = 0;
        while (std::getline(ss, val, ',') && col < MAP_WIDTH) {
            tileMap[row][col++] = std::stoi(val);
        }
        row++;
    }
}

void renderTileMap(SDL_Renderer* renderer, SDL_Texture* tileTexture) {
    for (int row = 0; row < MAP_HEIGHT; ++row) {
        for (int col = 0; col < MAP_WIDTH; ++col) {
            int tileID = tileMap[row][col];
            if (tileID < 0) continue;

            SDL_Rect src = getTileSrcRect(tileID);
            SDL_Rect dst = {
                col * TILE_SIZE - camera.x,
                row * TILE_SIZE - camera.y,
                TILE_SIZE,
                TILE_SIZE
            };

            if (dst.x + TILE_SIZE >= 0 && dst.x < SCREEN_WIDTH && dst.y + TILE_SIZE >= 0 && dst.y < SCREEN_HEIGHT) {
                std::cout << "Drawing tile " << tileID << " at (" << col << "," << row << ")\n";

                SDL_RenderCopy(renderer, tileTexture, &src, &dst);
            }
        }
    }
}

// Camera variables
const int BUFFER_ZONE_X = 200; // Horizontal buffer before camera moves
const int BUFFER_ZONE_Y = 100; // Vertical buffer before camera moves

// Camera follows player but with buffer zone
void updateCameraPosition(const Entity& player) {
    // Horizontal camera follow
    if (player.transform.x < camera.x + BUFFER_ZONE_X) {
        camera.x = std::max(0, player.transform.x - BUFFER_ZONE_X); // Keep within map bounds
    } else if (player.transform.x + player.transform.width > camera.x + camera.width - BUFFER_ZONE_X) {
        camera.x = std::min(MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH, player.transform.x + player.transform.width - SCREEN_WIDTH + BUFFER_ZONE_X);
    }

    // Vertical camera follow
    if (player.transform.y < camera.y + BUFFER_ZONE_Y) {
        camera.y = std::max(0, player.transform.y - BUFFER_ZONE_Y); // Keep within map bounds
    } else if (player.transform.y + player.transform.height > camera.y + camera.height - BUFFER_ZONE_Y) {
        camera.y = std::min(MAP_HEIGHT * TILE_SIZE - SCREEN_HEIGHT, player.transform.y + player.transform.height - SCREEN_HEIGHT + BUFFER_ZONE_Y);
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Tilemap ECS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* tileTexture = IMG_LoadTexture(renderer, "tilesheet.png");
    int texW, texH;
    SDL_QueryTexture(tileTexture, NULL, NULL, &texW, &texH);
    tilesheetCols = texW / TILE_SIZE;



    loadMap("map.csv");

    // Player Entity
    Entity player;
    // player.transform = {100, 100, 70, 70};
    player.transform = {10 * TILE_SIZE, 15 * TILE_SIZE, 70, 70}; // Move to row 15, col 10

    player.sprite.texture = tileTexture;
    player.sprite.srcRect = getTileSrcRect(1); // Use tile 1 as player

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        // Basic movement with arrow keys
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_RIGHT]) player.transform.x += 5;
        if (keys[SDL_SCANCODE_LEFT])  player.transform.x -= 5;

        // Camera follows player
        updateCameraPosition(player);
        /*
        camera.x = player.transform.x + player.transform.width / 2 - camera.width / 2;
        camera.y = player.transform.y + player.transform.height / 2 - camera.height / 2;
        camera.x = std::max(0, std::min(camera.x, MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH));
        camera.y = std::max(0, std::min(camera.y, MAP_HEIGHT * TILE_SIZE - SCREEN_HEIGHT));
        */

        SDL_RenderClear(renderer);

        renderTileMap(renderer, tileTexture);

        // Render player
        SDL_Rect dst = {
            player.transform.x - camera.x,
            player.transform.y - camera.y,
            player.transform.width,
            player.transform.height
        };
        SDL_RenderCopy(renderer, player.sprite.texture, &player.sprite.srcRect, &dst);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyTexture(tileTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

