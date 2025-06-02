#include "Engine.h"
#include "MenuState.h"

int main(int argc, char* argv[]) {
    Engine engine;

    if (!engine.init()) {
        return EXIT_FAILURE;
    }

    // Start in MenuState
    std::shared_ptr<GameState> state = std::make_shared<MenuState>();

    engine.run(state);

    engine.shutdown();

    return EXIT_SUCCESS;
}
