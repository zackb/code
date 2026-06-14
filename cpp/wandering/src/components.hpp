#pragma once
#include "resources.hpp"
#include <raylib.h>
#include <vector>

// Plain-old-data components. Systems (see systems.hpp) operate on entities that
// carry combinations of these. Names are deliberately distinct from raylib's
// own types (raylib already defines a `Transform`), hence `Spatial`.

// World-space placement.
struct Spatial {
    Vector3 position{0.0f, 0.0f, 0.0f};
    Vector3 scale{1.0f, 1.0f, 1.0f};
};

// Per-frame velocity. Horizontal components are overwritten each frame by the
// input/AI systems; the vertical component persists so gravity can accumulate.
struct Velocity {
    Vector3 value{0.0f, 0.0f, 0.0f};
};

// Control frame: the heading that movement input is interpreted relative to.
// For the player this is the camera yaw.
struct Heading {
    float yaw = 0.0f;
};

// Locomotion parameters and transient movement state.
struct MovementStats {
    float walkSpeed = 10.0f;
    float runSpeed = 20.0f;
    float jumpForce = 12.0f;
    float gravity = 30.0f;
    bool grounded = false;
    bool running = false;
    bool moving = false;
};

// Visual representation. `modelYaw` is the smoothed facing direction the model
// is drawn at (radians). yOffset/yawOffset adapt a model whose origin/forward
// axis differs from the engine convention.
struct Renderable {
    ModelHandle model = INVALID_MODEL;
    Color tint = WHITE;
    float modelYaw = 0.0f;
    float yOffset = 0.0f;
    float yawOffset = 0.0f;
};

// Animation playback state. Indices come from the model's AnimSet in the cache.
struct AnimState {
    int current = 0;
    int frame = 0;
};

// Tag: this entity is driven by keyboard/mouse input.
struct PlayerControl {};

// Tag: this static entity still needs its Y snapped to the terrain surface,
// which can only happen once the chunk beneath it has loaded.
struct GroundSnap {};

// Walks a closed loop of waypoints. (Used from Phase 2 onward.)
struct AIController {
    std::vector<Vector3> waypoints;
    int target = 0;
    float speed = 6.0f;
};
