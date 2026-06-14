#include "camera.hpp"
#include "components.hpp"
#include <raymath.h>

ThirdPersonCamera::ThirdPersonCamera(entt::registry& reg, entt::entity target)
    : reg(reg), target(target), distance(12.0f), pitch(0.3f), yaw(0.0f) {

    Vector3 targetPos = reg.get<Spatial>(target).position;

    camera.position = {0.0f, 10.0f, 10.0f};
    camera.target = targetPos;
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    DisableCursor(); // Capture mouse
}

void ThirdPersonCamera::Update(float dt) {
    Vector2 mouseDelta = GetMouseDelta();
    float lookSensitivity = 0.003f;

    yaw -= mouseDelta.x * lookSensitivity;
    pitch += mouseDelta.y * lookSensitivity;

    // clamp pitch to avoid flipping and passing through floor
    if (pitch > 1.5f)
        pitch = 1.5f;
    if (pitch < -0.2f)
        pitch = -0.2f;

    // feed the camera yaw back to the player as its control heading
    reg.get<Heading>(target).yaw = yaw;

    // zoom
    float wheel = GetMouseWheelMove();
    distance -= wheel * 2.0f;
    if (distance < 3.0f)
        distance = 3.0f;
    if (distance > 30.0f)
        distance = 30.0f;

    // calculate camera position based on player and spherical coordinates
    Vector3 playerPos = reg.get<Spatial>(target).position;

    // an offset for where the camera looks
    Vector3 targetPos = Vector3Add(playerPos, {0.0f, 1.5f, 0.0f});
    camera.target = targetPos;

    float horizontalDistance = distance * cosf(pitch);
    float verticalDistance = distance * sinf(pitch);

    camera.position.x = targetPos.x - horizontalDistance * cosf(yaw);
    camera.position.y = targetPos.y + verticalDistance;
    camera.position.z = targetPos.z + horizontalDistance * sinf(yaw);
}

void ThirdPersonCamera::BeginMode() const { BeginMode3D(camera); }

void ThirdPersonCamera::EndMode() const { EndMode3D(); }
