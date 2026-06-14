#pragma once
#include "components.hpp"
#include "resources.hpp"
#include <entt/entt.hpp>
#include <raylib.h>

class World;

// Entity spawn helpers ------------------------------------------------------

// The player: keyboard/mouse controlled animated character.
entt::entity spawnPlayer(entt::registry& reg, ResourceCache& cache, Vector3 pos, const std::string& modelPath);

// An NPC that walks a waypoint loop. Y of each point is sampled from terrain.
entt::entity spawnNPC(entt::registry& reg, ResourceCache& cache, const World& world, const std::string& modelPath,
                      std::vector<Vector3> waypoints, float speed);

// A static, non-animated prop placed at a fixed location.
entt::entity spawnProp(entt::registry& reg, ResourceCache& cache, const std::string& modelPath, Vector3 pos,
                       float yaw, float scale);

// Reads the authored "npcs" and "models" arrays from the map file and spawns
// the corresponding entities. The terrain/prop sections are handled by World.
void loadScene(entt::registry& reg, ResourceCache& cache, const World& world, const std::string& mapPath);

// Systems -------------------------------------------------------------------

// Reads WASD/shift/space and writes Velocity + MovementStats on player entities.
void playerInputSystem(entt::registry& reg, float dt);

// Drives waypoint-following NPCs: writes horizontal Velocity + MovementStats.
void aiSystem(entt::registry& reg, float dt);

// Integrates Velocity into Spatial, applies gravity, snaps to terrain, and
// smooths each mover's facing (Renderable.modelYaw) toward its travel direction.
void movementSystem(entt::registry& reg, const World& world, float dt);

// Selects the animation clip from movement state and advances playback.
void animationSystem(entt::registry& reg, ResourceCache& cache, float dt);

// Snaps GroundSnap-tagged static entities to the terrain once their chunk has
// loaded, then drops the tag.
void groundSnapSystem(entt::registry& reg, const World& world);

// Draws every Spatial+Renderable entity.
void renderSystem(entt::registry& reg, ResourceCache& cache);
