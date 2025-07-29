#include "args.h"
#include "runner/daemon.h"

#include <cstdlib>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

using std::format;

int main(int argc, const char* argv[]) {

    // parse command line arguments
    Args args(argc, argv);

    if (args.daemon()) {
        std::cout << "Using database at: " << args.dbPath() << "\n";
        std::cout << "Starting server at: http://" << args.host() << ":" << args.port() << "\n";
        Daemon daemon(args);
        daemon.run();
    } else {

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
        auto startServer = ftxui::Button("Start Server", [&] {
            // Convert portRef to int and start the server
            int portValue = std::stoi(portRef);
            std::cout << format("Starting server at %s:%d...\n", args.host().c_str(), portValue);
            // srv.listen();
        });

        auto serverComponents = Container::Vertical({host, port, startServer});

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
    }

    return EXIT_SUCCESS;
}
