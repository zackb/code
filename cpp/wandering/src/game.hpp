#pragma once
#include "camera.hpp"
#include "resources.hpp"
#include "world.hpp"
#include <entt/entt.hpp>
#include <memory>
#include <string>

class Game {
public:
    Game();
    ~Game();

    void Initialize(int width, int height, const std::string& title);
    void Run();

private:
    void Update();
    void Draw();

    entt::registry registry;
    std::unique_ptr<ResourceCache> resources;
    std::unique_ptr<World> world;
    std::unique_ptr<ThirdPersonCamera> camera;
    entt::entity playerEntity;
    bool isRunning;
};
