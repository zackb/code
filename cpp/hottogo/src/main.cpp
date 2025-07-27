#include "args.h"
#include "server/server.h"
#include "sig.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <cstdlib>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

using std::format;

int main(int argc, const char* argv[]) {

    // parse command line arguments
    Args args(argc, argv);

    // http server with database
    std::cout << "Using database at: " << args.dbPath() << "\n";
    Server srv(args.dbPath());

    // handle signals
    setSignalHandler([&](int) {
        std::cout << "Stopping HTTP server...\n";
        srv.stop();
    });

    // start the http server
    std::cout << format("Starting HTTP server at: %s:%d...\n", args.host(), args.port());
    srv.listen(args.host(), args.port());

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

    auto host = ftxui::Input(args.host(), "host");
    auto portRef = std::to_string(args.port());
    auto port = ftxui::Input(&portRef, "port");
    auto serverComponents = Container::Vertical({host, port});
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
            // Radiobox(&serverEntries, &serverSelected),
            serverComponents,
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
