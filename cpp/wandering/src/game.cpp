#include "game.hpp"
#include "components.hpp"
#include "debug.hpp"
#include "systems.hpp"
#include <raylib.h>

Game::Game() : playerEntity(entt::null), isRunning(false) {}

Game::~Game() {
    // Tear down GPU-owning resources while the GL context is still alive,
    // before CloseWindow() destroys it.
    camera.reset();
    registry.clear();
    resources.reset();
    world.reset();
    CloseWindow();
}

void Game::Initialize(int width, int height, const std::string& title) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);

    resources = std::make_unique<ResourceCache>();
    world = std::make_unique<World>("assets/map.json");

    playerEntity = spawnPlayer(registry, *resources, Vector3{0.0f, 1.0f, 0.0f}, "assets/zlorp.glb");
    camera = std::make_unique<ThirdPersonCamera>(registry, playerEntity);

    // spawn authored NPCs and static scenery from the map file
    loadScene(registry, *resources, *world, "assets/map.json");

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

    if (IsKeyPressed(KEY_ESCAPE)) {
        isRunning = false;
    }
    if (IsKeyPressed(KEY_Q)) {
        debug::enabled = !debug::enabled;
    }

    playerInputSystem(registry, dt);
    aiSystem(registry, dt);
    movementSystem(registry, *world, dt);
    camera->Update(dt);
    animationSystem(registry, *resources, dt);

    world->Update(registry.get<Spatial>(playerEntity).position, dt);
    groundSnapSystem(registry, *world);
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(SKYBLUE);

    camera->BeginMode();
    {
        world->Draw();
        renderSystem(registry, *resources);
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
