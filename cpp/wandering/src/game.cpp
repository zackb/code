#include "game.hpp"
#include "debug.hpp"
#include <raylib.h>

Game::Game() : isRunning(false) {}

Game::~Game() { CloseWindow(); }

void Game::Initialize(int width, int height, const std::string& title) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);

    world = std::make_unique<World>("assets/map.json");
    player = std::make_unique<Player>(Vector3{0.0f, 1.0f, 0.0f});
    camera = std::make_unique<ThirdPersonCamera>(player.get());

    isRunning = true;
}

void Game::Run() {
    while (!WindowShouldClose() && isRunning) {
        Update();
        Draw();
    }
}

void Game::Update() {
    float dt = GetFrameTime();

    // close window if esc pressed
    if (IsKeyPressed(KEY_ESCAPE)) {
        isRunning = false;
    }

    if (IsKeyPressed(KEY_Q)) {
        debug::enabled = !debug::enabled;
    }

    player->Update(dt, world->GetTerrainHeightAt(player->GetPosition().x, player->GetPosition().z));
    camera->Update(dt);
    world->Update(player->GetPosition(), dt);
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(SKYBLUE);

    camera->BeginMode();
    {
        world->Draw();
        player->Draw();
    }
    camera->EndMode();

    if (debug::enabled) {
        DrawFPS(10, 10);
        DrawText("WASD to Move, SPACE to Jump, MOUSE to Rotate Camera", 10, 30, 20, DARKGRAY);
    }

    // draw a crosshair
    DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 2, WHITE);

    EndDrawing();
}
