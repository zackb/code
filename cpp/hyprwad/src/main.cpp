#include "ui.hpp"
#include <algorithm>
#include <imgui.h>
#include <iostream>
#include <vector>

class Selector : public Frame {

public:
    struct Choice {
        std::string id;
        std::string display;
        bool selected = false;
    };

    void add(Choice& choice) { choices.emplace_back(choice); }
    void add(Choice&& choice) { choices.emplace_back(choice); }
    void setSelected(int index) { selected = index; }

    virtual bool render() {
        ImGui::Begin("Select",
                     nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoSavedSettings);
        // ImGui::Separator();

        int clicked = -1;

        for (int i = 0; i < choices.size(); i++) {
            if (ImGui::Selectable(choices[i].display.c_str(), selected == i)) {
                selected = i;
                clicked = i;
            }
        }

        ImGui::End();

        if (clicked >= 0) {
            std::cout << choices[clicked].id << std::endl;
            return false;
        }

        return true;
    }

private:
    int selected = 0;
    std::vector<Choice> choices;
};

std::vector<Selector::Choice> parseArgsOrStdin(int argc, const char* argv[]) {
    std::vector<Selector::Choice> items;

    auto parseLine = [](const std::string& line) -> Selector::Choice {
        Selector::Choice item;
        std::string s = line;

        if (!s.empty() && s.back() == '*') {
            item.selected = true;
            s.pop_back();
        }

        size_t colon = s.find(':');
        if (colon != std::string::npos) {
            item.id = s.substr(0, colon);
            item.display = s.substr(colon + 1);
        } else {
            item.id = s;
            item.display = s;
        }

        return item;
    };

    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            items.push_back(parseLine(argv[i]));
        }
    } else {
        std::string line;
        while (std::getline(std::cin, line)) {
            if (!line.empty())
                items.push_back(parseLine(line));
        }
    }

    // Ensure only one item is selected
    bool anySelected =
        std::any_of(items.begin(), items.end(), [](const Selector::Choice& item) { return item.selected; });

    if (!anySelected && !items.empty()) {
        items[0].selected = true; // Default to the first
    }

    return items;
}

void usage() {
    fprintf(stderr, R"(Usage:
  hyprwad [OPTIONS] [id[:displayName][*]]...

Description:
  A simple Wayland panel using ImGui to present selectable options.

  You can pass a list of items directly as command-line arguments, where each
  item is a tuple in the form:

      id[:displayName][*]

  - `id`           : Required identifier string (used internally)
  - `displayName`  : Optional label to show in the UI (defaults to id)
  - `*`            : Optional suffix to mark this item as initially selected

  Examples:
    hyprwad performance:Performance* balanced:Balanced powersave:PowerSaver
    hyprwad wifi0:Home wifi1:Work wifi2:Other

Alternatively, if no arguments are passed, options can be provided via stdin:

  echo "wifi0:Home*" | hyprwad
  echo -e "wifi0:Home*\nwifi1:Work\nwifi2:Other" | hyprwad

Options:
  -h, --help       Show this help message
)");
}

int main(int argc, const char* argv[]) {
    std::cout << argv[1] << std::endl;
    if (argc == 2 && !strncmp(argv[1], "--help", strlen(argv[1]))) {
        usage();
        return 1;
    }
    Selector frame;
    UI ui;
    ui.init("HyprWad");
    auto choices = parseArgsOrStdin(argc, argv);
    int i = 0;
    for (auto& choice : choices) {
        frame.add({choice.id, choice.display});
        if (choice.selected) {
            frame.setSelected(i);
        }
        ++i;
    }
    ui.run(frame);
    ui.destroy();
    return 0;
}
