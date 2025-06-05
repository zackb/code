// A basic Doom-like 2.5D raycasting engine using OpenGL and SDL2

#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
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

void drawVerticalLine(int x, int drawStart, int drawEnd, float shade) {
    glColor3f(shade, shade, shade);
    glBegin(GL_LINES);
    glVertex2f((x / float(SCREEN_WIDTH)) * 2 - 1, (drawStart / float(SCREEN_HEIGHT)) * -2 + 1);
    glVertex2f((x / float(SCREEN_WIDTH)) * 2 - 1, (drawEnd / float(SCREEN_HEIGHT)) * -2 + 1);
    glEnd();
}

void renderFrame(const Player& player) {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        float cameraX = 2 * x / float(SCREEN_WIDTH) - 1;
        float rayAngle = player.angle + cameraX * FOV / 2.0f;

        float rayDirX = cos(rayAngle);
        float rayDirY = sin(rayAngle);

        float distance = 0.0f;
        float step = 0.01f;
        bool hit = false;

        int mapX = 0, mapY = 0;
        float testX, testY;

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
            }
        }

        float correctedDist = distance * cos(rayAngle - player.angle);
        int lineHeight = int(SCREEN_HEIGHT / correctedDist);
        int drawStart = std::max(0, -lineHeight / 2 + SCREEN_HEIGHT / 2);
        int drawEnd = std::min(SCREEN_HEIGHT - 1, lineHeight / 2 + SCREEN_HEIGHT / 2);

        float shade = 1.0f - std::min(correctedDist / 10.0f, 1.0f);
        drawVerticalLine(x, drawStart, drawEnd, shade);
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("OpenGL Raycaster",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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

        renderFrame(player);
        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
