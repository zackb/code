#include "Signal.h"
#include "server/Server.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <cstdlib>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

int main(int argc, const char* argv[]) {

    // http server with database
    Server srv("cap.db");

    // handle signals
    setSignalHandler([&](int) {
        std::cout << "Stopping HTTP server..." << std::endl;
        srv.stop();
    });

    // start the http server
    std::cout << "Starting HTTP server on port 8080..." << std::endl;
    srv.listen("0.0.0.0", 8080);

    // ui
    using namespace ftxui;
    std::vector<std::string> tabLabels{
        "Home",
        "Server",
        "Settings",
    };
    int tabSelected = 0;
    auto tabToggle = Toggle(&tabLabels, &tabSelected);

    std::vector<std::string> homeEntries{
        "Forest",
        "Water",
        "I don't know",
    };
    int homeSelected = 0;

    std::vector<std::string> serverEntries{
        "Hello",
        "Hi",
        "Hay",
    };
    int serverSelected = 0;

    std::vector<std::string> settingsEntries{
        "Table",
        "Nothing",
        "Is",
        "Empty",
    };
    int settingsSelected = 0;

    auto tabContainer = Container::Tab(
        {
            Radiobox(&homeEntries, &homeSelected),
            Radiobox(&serverEntries, &serverSelected),
            Radiobox(&settingsEntries, &settingsSelected),
        },
        &tabSelected);

    auto container = Container::Vertical({
        tabToggle,
        tabContainer,
    });

    auto renderer = Renderer(container, [&] {
        return vbox({
                   tabToggle->Render(),
                   separator(),
                   tabContainer->Render(),
               }) |
               border;
    });

    auto screen = ScreenInteractive::Fullscreen();
    screen.Loop(renderer);

    return EXIT_SUCCESS;
}
