#include "Engine.h"
#include "MenuState.h"
#include <memory>

int main(int argc, char* argv[]) {
    Engine engine;

    if (!engine.init()) {
        return EXIT_FAILURE;
    }

    // Start in MenuState
    std::unique_ptr<GameState> state = std::make_unique<MenuState>();

    engine.run(std::move(state));

    engine.shutdown();

    return EXIT_SUCCESS;
}
