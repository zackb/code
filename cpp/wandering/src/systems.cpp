#include "systems.hpp"
#include "debug.hpp"
#include "world.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <raymath.h>

using json = nlohmann::json;

// --- Spawn helpers ---------------------------------------------------------

entt::entity spawnPlayer(entt::registry& reg, ResourceCache& cache, Vector3 pos, const std::string& modelPath) {
    entt::entity e = reg.create();
    reg.emplace<Spatial>(e, Spatial{pos, {1.0f, 1.0f, 1.0f}});
    reg.emplace<Velocity>(e);
    reg.emplace<Heading>(e);
    reg.emplace<MovementStats>(e);
    reg.emplace<AnimState>(e);
    reg.emplace<PlayerControl>(e);

    ModelHandle h = cache.loadAnimated(modelPath);
    // zlorp models have their origin at the body center with feet 1 unit below
    // and face +X, so they need a 90deg yaw offset to align with the engine.
    reg.emplace<Renderable>(e, Renderable{h, WHITE, 0.0f, 1.0f, 90.0f});
    return e;
}

entt::entity spawnNPC(entt::registry& reg, ResourceCache& cache, const World& world, const std::string& modelPath,
                      std::vector<Vector3> waypoints, float speed) {
    // Snap each waypoint to the terrain surface (foot offset of 1 unit).
    for (auto& wp : waypoints) {
        wp.y = world.GetTerrainHeightAt(wp.x, wp.z) + 1.0f;
    }

    Vector3 start = waypoints.empty() ? Vector3{0.0f, 1.0f, 0.0f} : waypoints[0];

    entt::entity e = reg.create();
    reg.emplace<Spatial>(e, Spatial{start, {1.0f, 1.0f, 1.0f}});
    reg.emplace<Velocity>(e);
    reg.emplace<MovementStats>(e);
    reg.emplace<AnimState>(e);
    reg.emplace<AIController>(e, AIController{std::move(waypoints), 0, speed});

    ModelHandle h = cache.loadAnimated(modelPath);
    reg.emplace<Renderable>(e, Renderable{h, WHITE, 0.0f, 1.0f, 90.0f});
    return e;
}

entt::entity spawnProp(entt::registry& reg, ResourceCache& cache, const std::string& modelPath, Vector3 pos,
                       float yaw, float scale) {
    entt::entity e = reg.create();
    reg.emplace<Spatial>(e, Spatial{pos, {scale, scale, scale}});

    ModelHandle h = cache.loadStatic(modelPath);
    reg.emplace<Renderable>(e, Renderable{h, WHITE, yaw, 0.0f, 0.0f});
    reg.emplace<GroundSnap>(e);
    return e;
}

void loadScene(entt::registry& reg, ResourceCache& cache, const World& world, const std::string& mapPath) {
    std::ifstream file(mapPath);
    if (!file.is_open()) {
        std::cerr << "loadScene: could not open map file: " << mapPath << std::endl;
        return;
    }

    json j;
    try {
        j = json::parse(file);
    } catch (const std::exception& e) {
        std::cerr << "loadScene: failed to parse map JSON: " << e.what() << std::endl;
        return;
    }

    // Animated NPCs that walk a loop of [x, z] waypoints.
    if (j.contains("npcs")) {
        for (const auto& n : j["npcs"]) {
            std::string model = n.value("model", std::string("assets/zlorp.glb"));
            float speed = n.value("speed", 6.0f);

            std::vector<Vector3> waypoints;
            if (n.contains("waypoints")) {
                for (const auto& w : n["waypoints"]) {
                    waypoints.push_back({w[0].get<float>(), 0.0f, w[1].get<float>()});
                }
            }
            spawnNPC(reg, cache, world, model, std::move(waypoints), speed);
        }
    }

    // Static GLB scenery placed at fixed [x, z] positions (y from terrain).
    if (j.contains("models")) {
        for (const auto& m : j["models"]) {
            std::string model = m["model"].get<std::string>();
            float x = m["position"][0].get<float>();
            float z = m["position"][1].get<float>();
            float yaw = m.value("yaw", 0.0f) * DEG2RAD;
            float scale = m.value("scale", 1.0f);
            float y = world.GetTerrainHeightAt(x, z);
            spawnProp(reg, cache, model, {x, y, z}, yaw, scale);
        }
    }
}

// --- Systems ---------------------------------------------------------------

void playerInputSystem(entt::registry& reg, float /*dt*/) {
    auto view = reg.view<PlayerControl, Heading, Velocity, MovementStats>();
    for (auto e : view) {
        auto& heading = view.get<Heading>(e);
        auto& vel = view.get<Velocity>(e);
        auto& stats = view.get<MovementStats>(e);

        Vector2 input = {0.0f, 0.0f};
        if (IsKeyDown(KEY_W))
            input.y += 1.0f;
        if (IsKeyDown(KEY_S))
            input.y -= 1.0f;
        if (IsKeyDown(KEY_A))
            input.x -= 1.0f;
        if (IsKeyDown(KEY_D))
            input.x += 1.0f;

        if (Vector2Length(input) > 0.0f) {
            input = Vector2Normalize(input);
            stats.moving = true;
            stats.running = IsKeyDown(KEY_LEFT_SHIFT);
        } else {
            stats.moving = false;
            stats.running = false;
        }

        // movement is relative to the control heading (camera yaw)
        Vector3 forward = {cosf(heading.yaw), 0.0f, -sinf(heading.yaw)};
        Vector3 right = {sinf(heading.yaw), 0.0f, cosf(heading.yaw)};

        if (stats.moving) {
            Vector3 moveDir = Vector3Add(Vector3Scale(forward, input.y), Vector3Scale(right, input.x));
            moveDir = Vector3Normalize(moveDir);
            float speed = stats.running ? stats.runSpeed : stats.walkSpeed;
            vel.value.x = moveDir.x * speed;
            vel.value.z = moveDir.z * speed;
        } else {
            vel.value.x = 0.0f;
            vel.value.z = 0.0f;
        }

        if (stats.grounded && IsKeyPressed(KEY_SPACE)) {
            vel.value.y = stats.jumpForce;
            stats.grounded = false;
        }
    }
}

void aiSystem(entt::registry& reg, float /*dt*/) {
    auto view = reg.view<AIController, Spatial, Velocity, MovementStats>();
    for (auto e : view) {
        auto& ai = view.get<AIController>(e);
        auto& spatial = view.get<Spatial>(e);
        auto& vel = view.get<Velocity>(e);
        auto& stats = view.get<MovementStats>(e);

        if (ai.waypoints.empty()) {
            vel.value.x = 0.0f;
            vel.value.z = 0.0f;
            stats.moving = false;
            continue;
        }

        Vector3 target = ai.waypoints[ai.target];
        float dx = target.x - spatial.position.x;
        float dz = target.z - spatial.position.z;
        float dist = sqrtf(dx * dx + dz * dz);

        const float arriveThreshold = 1.5f;
        if (dist < arriveThreshold) {
            ai.target = (ai.target + 1) % static_cast<int>(ai.waypoints.size());
            vel.value.x = 0.0f;
            vel.value.z = 0.0f;
            stats.moving = false;
        } else {
            vel.value.x = (dx / dist) * ai.speed;
            vel.value.z = (dz / dist) * ai.speed;
            stats.moving = true;
            stats.running = false;
        }
    }
}

void movementSystem(entt::registry& reg, const World& world, float dt) {
    auto view = reg.view<Spatial, Velocity, MovementStats, Renderable>();
    for (auto e : view) {
        auto& spatial = view.get<Spatial>(e);
        auto& vel = view.get<Velocity>(e);
        auto& stats = view.get<MovementStats>(e);
        auto& rend = view.get<Renderable>(e);

        float groundHeight = world.GetTerrainHeightAt(spatial.position.x, spatial.position.z);

        // gravity + integration (horizontal velocity is set by input/AI systems)
        vel.value.y -= stats.gravity * dt;
        spatial.position.x += vel.value.x * dt;
        spatial.position.y += vel.value.y * dt;
        spatial.position.z += vel.value.z * dt;

        // terrain collision; the model origin sits `yOffset` above the feet
        float foot = spatial.position.y - rend.yOffset;
        bool wasGrounded = stats.grounded;
        if (foot <= groundHeight) {
            spatial.position.y = groundHeight + rend.yOffset;
            vel.value.y = 0.0f;
            stats.grounded = true;
        } else {
            // downhill snapping: avoid micro-falling jitter on descending slopes
            const float snapDistance = 0.5f;
            if (wasGrounded && vel.value.y <= 0.0f && foot <= groundHeight + snapDistance) {
                spatial.position.y = groundHeight + rend.yOffset;
                vel.value.y = 0.0f;
                stats.grounded = true;
            } else {
                stats.grounded = false;
            }
        }

        // smoothly turn the model to face its horizontal travel direction
        if (fabsf(vel.value.x) > 0.01f || fabsf(vel.value.z) > 0.01f) {
            float targetYaw = atan2f(-vel.value.z, vel.value.x);
            float diff = targetYaw - rend.modelYaw;
            while (diff < -PI)
                diff += 2.0f * PI;
            while (diff > PI)
                diff -= 2.0f * PI;
            rend.modelYaw += diff * 15.0f * dt;
        }
    }
}

void animationSystem(entt::registry& reg, ResourceCache& cache, float /*dt*/) {
    auto view = reg.view<Renderable, AnimState, MovementStats>();
    for (auto e : view) {
        auto& rend = view.get<Renderable>(e);
        auto& anim = view.get<AnimState>(e);
        auto& stats = view.get<MovementStats>(e);

        const AnimSet& set = cache.anims(rend.model);
        if (set.count == 0)
            continue;

        int next;
        if (!stats.grounded) {
            next = set.jumpIndex;
        } else if (stats.moving) {
            next = stats.running ? set.runIndex : set.walkIndex;
        } else {
            next = set.idleIndex;
        }

        if (next != anim.current) {
            anim.current = next;
            anim.frame = 0;
        }

        anim.frame++;
        UpdateModelAnimation(cache.model(rend.model), set.anims[anim.current], anim.frame);

        // hold the last frame of the jump animation while airborne; loop others
        if (anim.current == set.jumpIndex) {
            if (anim.frame >= set.anims[anim.current].keyframeCount - 1) {
                anim.frame = set.anims[anim.current].keyframeCount - 1;
            }
        } else if (anim.frame >= set.anims[anim.current].keyframeCount) {
            anim.frame = 0;
        }
    }
}

void groundSnapSystem(entt::registry& reg, const World& world) {
    // Collect first; we mutate the registry (remove the tag) while iterating.
    std::vector<entt::entity> ready;
    auto view = reg.view<GroundSnap, Spatial>();
    for (auto e : view) {
        auto& spatial = view.get<Spatial>(e);
        if (world.IsLoadedAt(spatial.position.x, spatial.position.z)) {
            spatial.position.y = world.GetTerrainHeightAt(spatial.position.x, spatial.position.z);
            ready.push_back(e);
        }
    }
    for (auto e : ready) {
        reg.remove<GroundSnap>(e);
    }
}

void renderSystem(entt::registry& reg, ResourceCache& cache) {
    auto view = reg.view<Spatial, Renderable>();
    for (auto e : view) {
        auto& spatial = view.get<Spatial>(e);
        auto& rend = view.get<Renderable>(e);

        Vector3 drawPos = spatial.position;
        drawPos.y -= rend.yOffset;
        float drawRot = (rend.modelYaw * RAD2DEG) + rend.yawOffset;
        DrawModelEx(cache.model(rend.model), drawPos, {0.0f, 1.0f, 0.0f}, drawRot, spatial.scale, rend.tint);

        if (debug::enabled && reg.all_of<MovementStats>(e)) {
            DrawCubeWires(spatial.position, 1.0f, 2.0f, 1.0f, RED);
        }
    }
}
