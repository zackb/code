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

GLuint wallTexture = 0;

SDL_Surface* loadSurface(const char* file) {
    SDL_Surface* surface = SDL_LoadBMP(file);
    if (!surface) {
        std::cerr << "Failed to load BMP: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    // Convert to 32-bit RGBA format for OpenGL
    SDL_Surface* formatted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(surface);
    return formatted;
}

GLuint loadTexture(const char* file) {
    SDL_Surface* surface = loadSurface(file);
    if (!surface)
        return 0;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);
    return tex;
}

void drawTexturedVerticalLine(int x, int drawStart, int drawEnd, float wallX, int side, float distance) {
    // wallX = exact hit position on the wall (0 to 1)
    // side = 0 if hit vertical wall (x-side), 1 if horizontal wall (y-side)
    // Use wallX to get texture coordinate
    // side used to flip texture coord for proper orientation

    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallTexture);

    float lineHeight = drawEnd - drawStart + 1;

    // Texture coordinates for vertical line:
    float texX = wallX;
    if (side == 1) {
        texX = 1.0f - texX; // flip texture horizontally on y-side walls for consistency
    }

    // render a quad strip for the vertical line at screen column x.
    // Screen x coords: convert to OpenGL -1 to 1 space horizontally
    float screenX = (x / float(SCREEN_WIDTH)) * 2.0f - 1.0f;

    // Calculate texture step per pixel on screen
    float texStep = 1.0f / lineHeight;

    // We'll draw a quad of width 2 / SCREEN_WIDTH in NDC (so one pixel wide)
    float pixelWidth = 2.0f / SCREEN_WIDTH;

    // Draw quad for vertical stripe
    glBegin(GL_QUADS);

    // Top vertex
    glTexCoord2f(texX, 0.0f);
    glVertex2f(screenX, (drawStart / float(SCREEN_HEIGHT)) * -2.0f + 1.0f);

    // Bottom vertex
    glTexCoord2f(texX, 1.0f);
    glVertex2f(screenX, (drawEnd / float(SCREEN_HEIGHT)) * -2.0f + 1.0f);

    // Bottom right vertex (next pixel)
    glTexCoord2f(texX, 1.0f);
    glVertex2f(screenX + pixelWidth, (drawEnd / float(SCREEN_HEIGHT)) * -2.0f + 1.0f);

    // Top right vertex (next pixel)
    glTexCoord2f(texX, 0.0f);
    glVertex2f(screenX + pixelWidth, (drawStart / float(SCREEN_HEIGHT)) * -2.0f + 1.0f);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawFloorAndCeiling(const Player& player) {
    // We'll draw flat colors for floor and ceiling using a full-screen quad with gradient

    // Ceiling - upper half
    glBegin(GL_QUADS);
    glColor3f(0.53f, 0.81f, 0.92f); // sky blue
    glVertex2f(-1, 1);
    glVertex2f(1, 1);
    glVertex2f(1, 0);
    glVertex2f(-1, 0);
    glEnd();

    // Floor - bottom half
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.3f, 0.3f); // dark gray floor
    glVertex2f(-1, 0);
    glVertex2f(1, 0);
    glVertex2f(1, -1);
    glVertex2f(-1, -1);
    glEnd();
}

void renderFrame(const Player& player) {
    glClear(GL_COLOR_BUFFER_BIT);

    drawFloorAndCeiling(player);

    // Precompute direction and camera plane
    float dirX = cos(player.angle);
    float dirY = sin(player.angle);
    float planeX = -dirY * tan(FOV / 2);
    float planeY = dirX * tan(FOV / 2);

    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        float cameraX = 2.0f * x / float(SCREEN_WIDTH) - 1.0f; // -1 to +1

        // Ray direction for this column
        float rayDirX = dirX + planeX * cameraX;
        float rayDirY = dirY + planeY * cameraX;

        int mapX = int(player.x);
        int mapY = int(player.y);

        float sideDistX;
        float sideDistY;

        float deltaDistX = (rayDirX == 0) ? 1e30f : std::abs(1 / rayDirX);
        float deltaDistY = (rayDirY == 0) ? 1e30f : std::abs(1 / rayDirY);

        int stepX;
        int stepY;

        bool hit = false;
        int side;

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (player.x - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0f - player.x) * deltaDistX;
        }

        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (player.y - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0f - player.y) * deltaDistY;
        }

        // DDA
        while (!hit) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) {
                hit = true;
                break;
            }

            if (worldMap[mapY][mapX] > 0) {
                hit = true;
            }
        }

        // Calculate perpendicular wall distance
        float perpWallDist;
        if (side == 0) {
            perpWallDist = (mapX - player.x + (1 - stepX) / 2.0f) / rayDirX;
        } else {
            perpWallDist = (mapY - player.y + (1 - stepY) / 2.0f) / rayDirY;
        }

        perpWallDist = std::max(perpWallDist, 0.01f);

        // Calculate line height
        int lineHeight = int(SCREEN_HEIGHT / perpWallDist);

        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0)
            drawStart = 0;

        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEnd >= SCREEN_HEIGHT)
            drawEnd = SCREEN_HEIGHT - 1;

        // Calculate wallX (exact hit position on the wall)
        float wallX;
        if (side == 0)
            wallX = player.y + perpWallDist * rayDirY;
        else
            wallX = player.x + perpWallDist * rayDirX;
        wallX -= floor(wallX);

        drawTexturedVerticalLine(x, drawStart, drawEnd, wallX, side, perpWallDist);
    }
}

void drawMinimap(const Player& player) {
    const float mapScale = 8.0f; // size of each map tile on minimap (pixels)
    const float minimapSizeX = MAP_WIDTH * mapScale;
    const float minimapSizeY = MAP_HEIGHT * mapScale;

    glPushMatrix();

    // Position minimap in top-left corner with some margin
    float margin = 10.0f;
    glTranslatef(-1.0f + margin / (SCREEN_WIDTH / 2.0f), 1.0f - margin / (SCREEN_HEIGHT / 2.0f), 0);
    glScalef(2.0f / SCREEN_WIDTH, -2.0f / SCREEN_HEIGHT, 1.0f);

    // Draw map background (semi-transparent black)
    glColor4f(0, 0, 0, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(minimapSizeX, 0);
    glVertex2f(minimapSizeX, minimapSizeY);
    glVertex2f(0, minimapSizeY);
    glEnd();

    // Draw map walls (white squares)
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (worldMap[y][x] > 0) {
                glColor3f(1, 1, 1);
                glBegin(GL_QUADS);
                glVertex2f(x * mapScale, y * mapScale);
                glVertex2f((x + 1) * mapScale, y * mapScale);
                glVertex2f((x + 1) * mapScale, (y + 1) * mapScale);
                glVertex2f(x * mapScale, (y + 1) * mapScale);
                glEnd();
            }
        }
    }

    // Draw player on minimap (red triangle)
    float px = player.x * mapScale;
    float py = player.y * mapScale;

    float size = 5.0f;
    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(px + size * cos(player.angle), py + size * sin(player.angle));
    glVertex2f(px - size * 0.5f * cos(player.angle + M_PI / 2), py - size * 0.5f * sin(player.angle + M_PI / 2));
    glVertex2f(px - size * 0.5f * cos(player.angle - M_PI / 2), py - size * 0.5f * sin(player.angle - M_PI / 2));
    glEnd();

    glPopMatrix();
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("OpenGL Raycaster Textured",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Enable alpha blending for minimap transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setup orthographic projection (NDC)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    wallTexture = loadTexture("assets/wall.bmp");
    if (!wallTexture) {
        std::cerr << "Failed to load wall texture, exiting." << std::endl;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

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
            float nx = player.x + cos(player.angle) * MOVE_SPEED;
            float ny = player.y + sin(player.angle) * MOVE_SPEED;
            // simple collision check
            if (worldMap[int(ny)][int(nx)] == 0) {
                player.x = nx;
                player.y = ny;
            }
        }
        if (keys[SDL_SCANCODE_S]) {
            float nx = player.x - cos(player.angle) * MOVE_SPEED;
            float ny = player.y - sin(player.angle) * MOVE_SPEED;
            if (worldMap[int(ny)][int(nx)] == 0) {
                player.x = nx;
                player.y = ny;
            }
        }
        if (keys[SDL_SCANCODE_A])
            player.angle -= ROT_SPEED;
        if (keys[SDL_SCANCODE_D])
            player.angle += ROT_SPEED;

        renderFrame(player);
        drawMinimap(player);

        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }

    glDeleteTextures(1, &wallTexture);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
