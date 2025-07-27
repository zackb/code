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

    // httplib::Client cli("https://ifconfig.co");
    // auto resp1 = cli.Get("/json");

    // std::cout << resp1->body << std::endl;

    Server srv("cap.db");

    setSignalHandler([&](int) {
        std::cout << "Stopping HTTP server..." << std::endl;
        srv.stop();
    });

    std::cout << "Starting HTTP server on port 8080..." << std::endl;
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    auto button = ftxui::Button("Click me", [] { std::cout << "Hello, FTXUI!\n"; });
    screen.Loop(button);

    srv.listen("0.0.0.0", 8080);

    return EXIT_SUCCESS;
}
