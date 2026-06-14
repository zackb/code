#pragma once
#include <entt/entt.hpp>
#include <raylib.h>

class ThirdPersonCamera {
public:
    ThirdPersonCamera(entt::registry& reg, entt::entity target);
    ~ThirdPersonCamera() = default;

    void Update(float dt);
    void BeginMode() const;
    void EndMode() const;

private:
    entt::registry& reg;
    entt::entity target;
    Camera3D camera;
    float distance;
    float pitch;
    float yaw;
};
