#pragma once

#include "../choice.hpp"
#include "../ui.hpp"
#include "imgui.h"
#include <vector>

class Selector : public Frame {

public:
    void add(Choice& choice) { choices.emplace_back(choice); }
    void add(Choice&& choice) { choices.emplace_back(choice); }
    void setSelected(int index) { selected = index; }

    bool RoundedSelectableFullWidth(const char* label, bool selected, float rounding = 6.0f);

    virtual bool render();

private:
    int selected = 0;
    std::vector<Choice> choices;
    ImVec4 highlightedColor = ImVec4(0.2f, 0.4f, 0.7f, 1.0f);
    ImVec4 selectedColor = ImVec4(0.2f, 0.4f, 0.7f, 0.4f);
};
