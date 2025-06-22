#pragma once

#include <raylib.h>

namespace ecs {

    struct Transform {
        Vector3 position;
        Vector4 rotation;
        Vector3 scale;

        Transform() : position({0.0f, 0.0f}), rotation(0.0f), scale({1.0f, 1.0f}) {}
    };

    struct Velocity {
        Vector3 velocity;

        Velocity() : velocity({0.0f, 0.0f, 0.0f}) {}
    };

    struct MeshComponent {
        Mesh mesh;
    };

}; // namespace ecs
