#include "mcamera.hpp"
#include "tex.hpp"
#include <cstdio>
#include <raylib.h>

void debug(Camera& camera, int cameraMode);

int main() {

    const int screenWidth = 1025;
    const int screenHeight = 768;
    InitWindow(screenWidth, screenHeight, "mimic");

    Camera camera = {0};
    camera.position = (Vector3){0.0f, 2.0f, 4.0f}; // Camera position
    camera.target = (Vector3){0.0f, 2.0f, 0.0f};   // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};       // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                           // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;        // Camera projection type

    int cameraMode = CAMERA_FIRST_PERSON;

    DisableCursor();  // Limit cursor to relative movement inside the window
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    Texture2D wallTexture = LoadTexture("assets/wall.png");
    if (wallTexture.id == 0) {
        printf("ERROR: Failed to load wall texture\n");
        return -1;
    }

    Model wallModel = tex::MakeVerticalWallModel(wallTexture, 5.0f, 1.0f, 1.0f);

    // Main game loop
    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_ONE)) {
            cameraMode = CAMERA_FREE;
            camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Reset roll
        }

        if (IsKeyPressed(KEY_TWO)) {
            cameraMode = CAMERA_FIRST_PERSON;
            camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Reset roll
        }

        if (IsKeyPressed(KEY_THREE)) {
            cameraMode = CAMERA_THIRD_PERSON;
            camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Reset roll
        }

        if (IsKeyPressed(KEY_FOUR)) {
            cameraMode = CAMERA_ORBITAL;
            camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Reset roll
        }

        // Switch camera projection
        if (IsKeyPressed(KEY_P)) {
            if (camera.projection == CAMERA_PERSPECTIVE) {
                // Create isometric view
                cameraMode = CAMERA_THIRD_PERSON;
                // Note: The target distance is related to the render distance in the orthographic projection
                camera.position = (Vector3){0.0f, 2.0f, -100.0f};
                camera.target = (Vector3){0.0f, 2.0f, 0.0f};
                camera.up = (Vector3){0.0f, 1.0f, 0.0f};
                camera.projection = CAMERA_ORTHOGRAPHIC;
                camera.fovy = 20.0f; // near plane width in CAMERA_ORTHOGRAPHIC
                cam::CameraYaw(&camera, -135 * DEG2RAD, true);
                cam::CameraPitch(&camera, -45 * DEG2RAD, true, true, false);
            } else if (camera.projection == CAMERA_ORTHOGRAPHIC) {
                // Reset to default view
                cameraMode = CAMERA_THIRD_PERSON;
                camera.position = (Vector3){0.0f, 2.0f, 10.0f};
                camera.target = (Vector3){0.0f, 2.0f, 0.0f};
                camera.up = (Vector3){0.0f, 1.0f, 0.0f};
                camera.projection = CAMERA_PERSPECTIVE;
                camera.fovy = 60.0f;
            }
        }

        UpdateCamera(&camera, cameraMode); // Update camera

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){32.0f, 32.0f}, LIGHTGRAY); // Draw ground
        // DrawCube((Vector3){-16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f, BLUE);           // Draw a blue wall
        // SetTextureWrap(wallTexture, TEXTURE_WRAP_REPEAT);
        // tex::DrawCubeTexture(wallTexture, (Vector3){-16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f, WHITE);
        // DrawModel(wallModel, (Vector3){-16.0f, 2.5f, 0.0f}, 1.0f, WHITE);
        // DrawModelEx(wallModel, (Vector3){-5.0f, 2.5f, 0.0f}, (Vector3){1, 0, 0}, 90.0f, (Vector3){1, 1, 1}, WHITE);
        // DrawModel(wallModel, (Vector3){-5.0f, 2.5f, 0.0f}, 1.0f, WHITE);
        // For repeating texture, you can calculate the texture rectangle
        float repeatU = 32.0f / 1.0f; // width / tileSize
        float repeatV = 5.0f / 1.0f;  // height / tileSize

        // Set texture to repeat
        SetTextureWrap(wallTexture, TEXTURE_WRAP_REPEAT);

        // Draw the textured cube with custom texture coordinates
        Rectangle textureSource = {0, 0, wallTexture.width * repeatU, wallTexture.height * repeatV};
        tex::DrawCubeTextureRec(wallTexture, textureSource, (Vector3){-16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f, WHITE);

        DrawCube((Vector3){16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f, LIME); // Draw a green wall
        DrawCube((Vector3){0.0f, 2.5f, 16.0f}, 32.0f, 5.0f, 1.0f, GOLD); // Draw a yellow wall

        // Draw player cube
        if (cameraMode == CAMERA_THIRD_PERSON) {
            DrawCube(camera.target, 0.5f, 0.5f, 0.5f, PURPLE);
            DrawCubeWires(camera.target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
        }

        EndMode3D();
        // debug(camera, cameraMode);
        EndDrawing();
    }

    CloseWindow();
    // UnloadTexture(wallTexture);

    return 0;
}

void debug(Camera& camera, int cameraMode) {

    // Draw info boxes
    DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(5, 5, 330, 100, BLUE);

    /*
    DrawText("Camera controls:", 15, 15, 10, BLACK);
    DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
    DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
    DrawText("- Camera mode keys: 1, 2, 3, 4", 15, 60, 10, BLACK);
    DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15, 75, 10, BLACK);
    DrawText("- Camera projection key: P", 15, 90, 10, BLACK);
    */

    DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(600, 5, 195, 100, BLUE);

    DrawText("Camera status:", 610, 15, 10, BLACK);
    DrawText(TextFormat("- Mode: %s",
                        (cameraMode == CAMERA_FREE)           ? "FREE"
                        : (cameraMode == CAMERA_FIRST_PERSON) ? "FIRST_PERSON"
                        : (cameraMode == CAMERA_THIRD_PERSON) ? "THIRD_PERSON"
                        : (cameraMode == CAMERA_ORBITAL)      ? "ORBITAL"
                                                              : "CUSTOM"),
             610,
             30,
             10,
             BLACK);
    DrawText(TextFormat("- Projection: %s",
                        (camera.projection == CAMERA_PERSPECTIVE)    ? "PERSPECTIVE"
                        : (camera.projection == CAMERA_ORTHOGRAPHIC) ? "ORTHOGRAPHIC"
                                                                     : "CUSTOM"),
             610,
             45,
             10,
             BLACK);
    DrawText(
        TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z),
        610,
        60,
        10,
        BLACK);
    DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z),
             610,
             75,
             10,
             BLACK);
    DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);
}
