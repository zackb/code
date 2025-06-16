#include "ui.hpp"
#include <imgui.h>
#include <iostream>
#include <vector>

class Selector : public Frame {

public:
    struct Choice {
        std::string id;
        std::string display;
    };

    void add(Choice& choice) { choices.emplace_back(choice); }
    void add(Choice&& choice) { choices.emplace_back(choice); }

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

int main(int argc, const char* argv[]) {
    Selector frame;
    UI ui;
    ui.init("HyprWad");
    frame.add({"help", "Help"});
    ui.run(frame);
    ui.destroy();
    return 0;
}
