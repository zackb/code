#include "game.hpp"

int main() {
    Game game;
    game.Initialize(1280, 720, "Wandering");
    game.Run();
    return 0;
}
