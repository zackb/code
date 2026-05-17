#pragma once
#include "camera.hpp"
#include "player.hpp"
#include "world.hpp"
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

    std::unique_ptr<Player> player;
    std::unique_ptr<ThirdPersonCamera> camera;
    std::unique_ptr<World> world;
    bool isRunning;
};
