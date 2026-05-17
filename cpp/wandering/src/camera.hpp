#pragma once
#include <raylib.h>

class Player;

class ThirdPersonCamera {
public:
    ThirdPersonCamera(Player* target);
    ~ThirdPersonCamera() = default;

    void Update(float dt);
    void BeginMode() const;
    void EndMode() const;

private:
    Player* target;
    Camera3D camera;
    float distance;
    float pitch;
    float yaw;
};
