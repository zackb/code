#include "mcamera.hpp"
#include "tex.hpp"
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

void debug(Camera& camera, int cameraMode);
void UpdateFirstPersonCamera(Camera& camera);
Model CreateSkyDome(Texture2D& texture, float radius);

int main() {

    /*
    const int screenWidth = 1025;
    const int screenHeight = 768;
    InitWindow(screenWidth, screenHeight, "mimic");
    */

    SetConfigFlags(FLAG_WINDOW_TOPMOST | FLAG_WINDOW_UNDECORATED);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Borderless fullscreen");

    Camera camera = {0};
    camera.position = (Vector3){0.0f, 2.0f, 4.0f}; // Camera position
    camera.target = (Vector3){0.0f, 2.0f, 0.0f};   // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};       // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                           // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;        // Camera projection type

    int cameraMode = CAMERA_FIRST_PERSON;

    DisableCursor();  // Limit cursor to relative movement inside the window
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // wall 1
    Texture2D wallTexture = LoadTexture("assets/wall.png");
    // Model wallModel = tex::MakeVerticalWallModel(wallTexture, 5.0f, 1.0f, 1.0f);

    // wall 2
    Mesh cubeMesh = GenMeshCube(1.0f, 5.0f, 32.0f);
    Model cubeModel = LoadModelFromMesh(cubeMesh);
    Shader wallShader = LoadShader("shaders/default.vs", "shaders/wall.fs");
    cubeModel.materials[0].shader = wallShader;

    // send uniforms like light direction
    int lightLoc = GetShaderLocation(wallShader, "lightDir");
    Vector3 lightDir = {0.0f, -1.0f, -1.0f};
    SetShaderValue(wallShader, lightLoc, &lightDir, SHADER_UNIFORM_VEC3);

    // floor
    Texture2D floorTexture = LoadTexture("assets/floor.png");

    // spidey
    Model arachnoid = LoadModel("assets/Arachnoid.obj");
    Texture2D texture = LoadTexture("assets/floor.png");
    if (texture.id != 0) {
        arachnoid.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    }

    // Sky
    Texture2D skyTexture = LoadTexture("assets/sky3.png");
    Model skyDome = CreateSkyDome(skyTexture, 100.0f);
    Shader skyShader = LoadShader("shaders/default.vs", "shaders/sky.fs");
    skyDome.materials[0].shader = skyShader;

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
        /*
        if (cameraMode == CAMERA_FIRST_PERSON) {
            UpdateFirstPersonCamera(camera);
        } else {
            UpdateCamera(&camera, cameraMode);
        }
        */

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // Draw sky sphere (make it huge and invert it so texture faces inward)
        // DrawSphereEx((Vector3){0, 0, 0}, 100.0f, 16, 16, SKYBLUE);
        // DrawCube((Vector3){0, 50, 0}, 200.0f, 100.0f, 200.0f, SKYBLUE);
        int timeLoc = GetShaderLocation(skyShader, "time");
        float time = GetTime();
        SetShaderValue(skyShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
        rlDisableBackfaceCulling();
        DrawModel(skyDome, (Vector3){0, 0, 0}, 1.0f, SKYBLUE);
        rlEnableBackfaceCulling();

        // --- Draw floor
        // DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){32.0f, 32.0f}, LIGHTGRAY); // Draw ground
        SetTextureWrap(floorTexture, TEXTURE_WRAP_REPEAT);

        // Calculate repetition
        float groundSize = 32.0f;
        float tileSize = 4.0f; // Adjust this for your desired tile size
        float repeatTimes = groundSize / tileSize;

        // Create texture source rectangle for repetition
        Rectangle textureSource = {0, 0, floorTexture.width * repeatTimes, floorTexture.height * repeatTimes};

        tex::DrawCubeTextureRec(floorTexture,
                                textureSource,
                                (Vector3){0.0f, -0.01f, 0.0f}, // Position slightly below y=0
                                32.0f,
                                0.02f,
                                32.0f, // Width, very thin height, depth
                                WHITE);

        // DrawCube((Vector3){-16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f, BLUE);           // Draw a blue wall

        // wall 1
        SetTextureWrap(wallTexture, TEXTURE_WRAP_REPEAT);
        float repeatU = 32.0f / 4.0f; // width / tileSize
        float repeatV = 5.0f / 4.0f;  // height / tileSize
        repeatTimes = 32.0f / 2.0f;   // = 16 repetitions
        textureSource = {0, 0, wallTexture.width * repeatU, wallTexture.height * repeatV};
        tex::DrawCubeTextureRec(wallTexture, textureSource, (Vector3){-16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f, WHITE);

        // wall 2
        // DrawCube((Vector3){16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f, LIME); // Draw a green wall
        int lightLoc = GetShaderLocation(wallShader, "lightDir");
        Vector3 wallPos = {16.0f, 2.5f, 0.0f}; // Center of your cube
        Vector3 light = {0.0f, -1.0f, -1.0f};
        SetShaderValue(wallShader, lightLoc, &light, SHADER_UNIFORM_VEC3);
        DrawModel(cubeModel, wallPos, 1.0f, WHITE);

        DrawCube((Vector3){0.0f, 2.5f, 16.0f}, 32.0f, 5.0f, 1.0f, GOLD); // Draw a yellow wall

        // Draw player cube
        if (cameraMode == CAMERA_THIRD_PERSON) {
            DrawCube(camera.target, 0.5f, 0.5f, 0.5f, PURPLE);
            DrawCubeWires(camera.target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
        }

        DrawModel(arachnoid, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
        EndMode3D();
        // debug(camera, cameraMode);
        EndDrawing();
    }

    UnloadTexture(wallTexture);
    UnloadTexture(floorTexture);
    UnloadTexture(skyTexture);
    UnloadModel(arachnoid);
    CloseWindow();

    return 0;
}

// Create sky dome model with inverted normals
Model CreateSkyDome(Texture2D& texture, float radius) {
    Mesh mesh = GenMeshSphere(radius, 32, 16);

    // Flip normals inward so texture faces the camera
    for (int i = 0; i < mesh.vertexCount; i++) {
        mesh.normals[i * 3 + 0] *= -1.0f;
        mesh.normals[i * 3 + 1] *= -1.0f;
        mesh.normals[i * 3 + 2] *= -1.0f;
    }

    /*
    // Reverse triangle winding order
    for (int i = 0; i < mesh.triangleCount; i++) {
        unsigned short temp = mesh.indices[i * 3 + 0];
        mesh.indices[i * 3 + 0] = mesh.indices[i * 3 + 2];
        mesh.indices[i * 3 + 2] = temp;
    }
    */

    UploadMesh(&mesh, false);
    Model model = LoadModelFromMesh(mesh);
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    return model;
}

// Custom first person camera update
void UpdateFirstPersonCamera(Camera& camera) {
    Vector2 mouseDelta = GetMouseDelta();
    static float yaw = 0.0f;
    static float pitch = 0.0f;

    yaw += mouseDelta.x * 0.002f;
    pitch -= mouseDelta.y * 0.002f;
    pitch = Clamp(pitch, -1.5f, 1.5f); // Limit pitch

    // Calculate camera direction
    camera.target.x = camera.position.x + cosf(yaw) * cosf(pitch);
    camera.target.y = camera.position.y + sinf(pitch);
    camera.target.z = camera.position.z + sinf(yaw) * cosf(pitch);

    // WASD movement
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3CrossProduct(forward, camera.up);
    float speed = 5.0f * GetFrameTime();

    if (IsKeyDown(KEY_W))
        camera.position = Vector3Add(camera.position, Vector3Scale(forward, speed));
    if (IsKeyDown(KEY_S))
        camera.position = Vector3Subtract(camera.position, Vector3Scale(forward, speed));
    if (IsKeyDown(KEY_A))
        camera.position = Vector3Subtract(camera.position, Vector3Scale(right, speed));
    if (IsKeyDown(KEY_D))
        camera.position = Vector3Add(camera.position, Vector3Scale(right, speed));

    // Update target
    camera.target.x = camera.position.x + cosf(yaw) * cosf(pitch);
    camera.target.y = camera.position.y + sinf(pitch);
    camera.target.z = camera.position.z + sinf(yaw) * cosf(pitch);
}

void debug(Camera& camera, int cameraMode) {

    // Draw info boxes
    DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(5, 5, 330, 100, BLUE);

    DrawText("Camera controls:", 15, 15, 10, BLACK);
    DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
    DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
    DrawText("- Camera mode keys: 1, 2, 3, 4", 15, 60, 10, BLACK);
    DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15, 75, 10, BLACK);
    DrawText("- Camera projection key: P", 15, 90, 10, BLACK);

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
