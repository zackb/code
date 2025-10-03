#include "choice.hpp"
#include "hyprland/hyprland_ipc.hpp"
#include "input.hpp"
#include "selection/selector.hpp"
#include "ui.hpp"
#include <imgui.h>
#include <iostream>

void usage() {
    fprintf(stderr, R"(Usage:
  hyprwat [OPTIONS] [id[:displayName][*]]...

Description:
  A simple Wayland panel using ImGui to present selectable options.

  You can pass a list of items directly as command-line arguments, where each
  item is a tuple in the form:

      id[:displayName][*]

  - `id`           : Required identifier string (used internally)
  - `displayName`  : Optional label to show in the UI (defaults to id)
  - `*`            : Optional suffix to mark this item as initially selected

  Examples:
    hyprwat performance:Performance* balanced:Balanced powersave:PowerSaver
    hyprwat wifi0:Home wifi1:Work wifi2:Other

Alternatively, if no arguments are passed, options can be provided via stdin:

  echo "wifi0:Home*" | hyprwat
  echo -e "wifi0:Home*\nwifi1:Work\nwifi2:Other" | hyprwat

Options:
  -h, --help       Show this help message
)");
}

int main(int argc, const char* argv[]) {

    std::cout.setf(std::ios::unitbuf);

    if (argc == 2 && !strncmp(argv[1], "--help", strlen(argv[1]))) {
        usage();
        return 1;
    }

    // initialize Hyprland IPC
    hyprland::Control ipc;

    // setup a selection frame
    Selector frame;
    UI ui;
    ui.init("HyprWat");
    ipc.send("dispatch moveactive mouse"); // center under mouse
    if (argc > 1) {
        // parse argv for choices
        auto choices = Input::parseArgv(argc, argv);
        int i = 0;
        for (auto& choice : choices) {
            frame.add({choice.id, choice.display});
            if (choice.selected) {
                frame.setSelected(i);
            }
            ++i;
        }
    } else {
        // parse stdin for choices asynchronously
        Input::parseStdin([&](Choice choice) { frame.add(choice); });
    }

    // listen for Hyprland events
    hyprland::Events events;
    events.start([&](const std::string& event) { std::cout << "Event: " << event << std::endl; });

    // run the UI loop
    ui.run(frame);
    events.stop();
    ui.destroy();
    return 0;
}
