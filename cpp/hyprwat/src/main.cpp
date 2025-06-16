#include "choice.hpp"
#include "input.hpp"
#include "ui.hpp"
#include <imgui.h>
#include <iostream>
#include <mutex>
#include <vector>

class Selector : public Frame {

public:
    void add(Choice& choice) { choices.emplace_back(choice); }
    void add(Choice&& choice) { choices.emplace_back(choice); }
    void setSelected(int index) { selected = index; }

    virtual bool render() {

        // lock for streaming stdin
        std::lock_guard<std::mutex> lock(Input::mutex);

        ImGui::Begin("Select",
                     nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                         ImGuiWindowFlags_AlwaysAutoResize);
        // ImGui::Separator();

        int clicked = -1;

        if (choices.size() == 0) {
            ImGui::Text("Loading...");
        } else {
            for (int i = 0; i < choices.size(); i++) {
                if (ImGui::Selectable(choices[i].display.c_str(), selected == i)) {
                    selected = i;
                    clicked = i;
                }
            }
        }

        ImGui::End();

        if (clicked >= 0) {
            std::cout << choices[clicked].id << std::endl;
            std::cout.flush();
            return false;
        }

        return true;
    }

private:
    int selected = 0;
    std::vector<Choice> choices;
};

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
    Selector frame;
    UI ui;
    ui.init("HyprWat");
    if (argc > 1) {
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
        Input::parseStdin([&](Choice choice) { frame.add(choice); });
    }
    ui.run(frame);
    ui.destroy();
    return 0;
}
