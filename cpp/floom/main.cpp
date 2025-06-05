// A basic Doom-like 2.5D raycasting engine in SDL2 (no OpenGL)
// sdl2_raycaster_game.cpp
// A basic Doom-like 2.5D raycasting engine in SDL2 (with wall textures)

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <cmath>
#include <iostream>

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr int MAP_WIDTH = 10;
constexpr int MAP_HEIGHT = 10;
constexpr float FOV = M_PI / 3.0f;
constexpr float MOVE_SPEED = 0.05f;
constexpr float ROT_SPEED = 0.05f;

int worldMap[MAP_HEIGHT][MAP_WIDTH] = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                                       {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                                       {1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
                                       {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
                                       {1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
                                       {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                                       {1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
                                       {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                                       {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                                       {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

struct Player {
    float x = 5.0f;
    float y = 5.0f;
    float angle = 0.0f;
};

void renderFrame(SDL_Renderer* renderer, SDL_Texture* wallTexture, const Player& player) {
    int texW, texH;
    SDL_QueryTexture(wallTexture, nullptr, nullptr, &texW, &texH);

    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        float cameraX = 2 * x / float(SCREEN_WIDTH) - 1;
        float rayAngle = player.angle + cameraX * FOV / 2.0f;

        float rayDirX = cos(rayAngle);
        float rayDirY = sin(rayAngle);

        float distance = 0.0f;
        float step = 0.01f;
        bool hit = false;
        int side = 0;

        float testX, testY;
        int mapX = 0, mapY = 0;

        while (!hit && distance < 20.0f) {
            distance += step;
            testX = player.x + rayDirX * distance;
            testY = player.y + rayDirY * distance;

            mapX = int(testX);
            mapY = int(testY);

            if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) {
                hit = true;
                distance = 20.0f;
            } else if (worldMap[mapY][mapX] > 0) {
                hit = true;
                float dx = testX - mapX - 0.5f;
                float dy = testY - mapY - 0.5f;
                side = fabs(dx) > fabs(dy) ? 0 : 1;
            }
        }

        float hitX = player.x + rayDirX * distance;
        float hitY = player.y + rayDirY * distance;

        float wallX = side == 0 ? hitY : hitX;
        wallX -= floor(wallX);

        int texX = int(wallX * texW);
        if (side == 0 && rayDirX > 0)
            texX = texW - texX - 1;
        if (side == 1 && rayDirY < 0)
            texX = texW - texX - 1;

        float correctedDist = distance * cos(rayAngle - player.angle);
        int lineHeight = int(SCREEN_HEIGHT / correctedDist);
        int drawStart = std::max(0, -lineHeight / 2 + SCREEN_HEIGHT / 2);
        int drawEnd = std::min(SCREEN_HEIGHT - 1, lineHeight / 2 + SCREEN_HEIGHT / 2);

        for (int y = drawStart; y < drawEnd; ++y) {
            int d = y * 256 - SCREEN_HEIGHT * 128 + lineHeight * 128;
            int texY = ((d * texH) / lineHeight) / 256;

            SDL_Rect srcRect = {texX, texY, 1, 1};
            SDL_Rect dstRect = {x, y, 1, 1};
            SDL_RenderCopy(renderer, wallTexture, &srcRect, &dstRect);
        }
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window =
        SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface* surface = SDL_LoadBMP("assets/wall.bmp");
    if (!surface) {
        std::cerr << "Failed to load texture: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Texture* wallTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    Player player;
    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
        }

        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W]) {
            player.x += cos(player.angle) * MOVE_SPEED;
            player.y += sin(player.angle) * MOVE_SPEED;
        }
        if (keys[SDL_SCANCODE_S]) {
            player.x -= cos(player.angle) * MOVE_SPEED;
            player.y -= sin(player.angle) * MOVE_SPEED;
        }
        if (keys[SDL_SCANCODE_A])
            player.angle -= ROT_SPEED;
        if (keys[SDL_SCANCODE_D])
            player.angle += ROT_SPEED;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        renderFrame(renderer, wallTexture, player);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(wallTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
