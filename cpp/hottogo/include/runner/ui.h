#pragma once

#include "args.h"
#include "runner.h"
#include "server/server.h"
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>

namespace ui {
    class UI : public Runner {

    public:
        explicit UI(Args& args);
        ~UI() override;
        UI(const UI&) = delete;
        UI& operator=(const UI&) = delete;
        UI(UI&&) = delete;

        void run() override;

    private:
        void buildLayout();
        void setupTabs();

        Args& args;
        Server server;

        // state
        bool running = false;
        int tabSelected = 0;
        std::vector<std::string> tabLabels = {"Home", "Server", "Settings"};

        int homeSelected = 0;
        int settingsSelected = 0;

        ftxui::ScreenInteractive screen;

        std::string portStr;
        ftxui::Component hostInput;
        ftxui::Component portInput;
        ftxui::Component startButton;

        ftxui::Component tabToggle;
        ftxui::Component tabContainer;
        ftxui::Component mainContainer;
        ftxui::Component renderer;

        ftxui::Component homeTab;
        ftxui::Component serverTab;
        ftxui::Component settingsTab;

        std::vector<std::string> homeEntries;
        std::vector<std::string> settingsEntries;
    };
}; // namespace ui
