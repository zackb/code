#include "runner/ui.h"
#include <ftxui/component/component.hpp>

namespace ui {
    using namespace ftxui;

    UI::UI(Args& args)
        : args(args), server(args), screen(ScreenInteractive::Fullscreen()), portStr(std::to_string(args.port())) {}

    UI::~UI() = default;

    void UI::run() {
        buildLayout();
        screen.Loop(renderer);
    }

    void UI::buildLayout() {
        tabToggle = Toggle(&tabLabels, &tabSelected);
        setupTabs();

        mainContainer = Container::Vertical({
            tabToggle,
            tabContainer,
        });

        renderer = Renderer(mainContainer, [&] {
            return vbox({
                       tabToggle->Render(),
                       separator(),
                       tabContainer->Render(),
                   }) |
                   border;
        });
    }

    void UI::setupTabs() {
        // Home tab
        homeEntries = {
            "Forest",
            "Water",
            "I don't know",
        };
        homeTab = Radiobox(&homeEntries, &homeSelected);

        // Server tab
        hostInput = Input(args.host(), "host");
        portInput = Input(&portStr, "port");

        startButton = Button("Start Server", [&] {
            if (running) {
                server.stop();
            } else {
                int portValue = std::stoi(portStr);
                args.port(portValue);

                std::cout << std::format("Starting server at {}:{}...\n", args.host(), portValue);
                server.init();
                server.start();
            }
        });

        serverTab = Container::Vertical({
            hostInput,
            portInput,
            startButton,
        });

        // Settings tab
        settingsEntries = {
            "Table",
            "Nothing",
            "Is",
            "Empty",
        };
        settingsTab = Radiobox(&settingsEntries, &settingsSelected);

        // Combine tabs
        tabContainer = Container::Tab(
            {
                homeTab,
                serverTab,
                settingsTab,
            },
            &tabSelected);
    }
}; // namespace ui
