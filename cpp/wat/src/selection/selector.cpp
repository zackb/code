#include "selector.hpp"
#include "../input.hpp"
#include "imgui_internal.h"
#include <iostream>
#include <mutex>

bool Selector::RoundedSelectableFullWidth(const char* label, bool selected, float rounding) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 padding = ImGui::GetStyle().FramePadding;
    ImVec2 labelSize = ImGui::CalcTextSize(label);

    // use the available content width instead of just text width
    float availableWidth = ImGui::GetContentRegionAvail().x;
    float minWidth = labelSize.x + padding.x * 2;
    float fullWidth = std::max(availableWidth, minWidth);

    ImVec2 size = ImVec2(fullWidth, labelSize.y + padding.y * 2);

    // create an invisible button first to handle interaction
    bool clicked = ImGui::InvisibleButton(label, size);
    bool hovered = ImGui::IsItemHovered();

    // draw the background if hovered or selected
    if (hovered || selected) {
        ImU32 color = ImGui::GetColorU32(selected && hovered ? highlightedColor
                                         : selected          ? selectedColor
                                                             : highlightedColor);
        ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), color, rounding);
    }

    // draw the text centered vertically, left-aligned horizontally
    ImVec2 textPos = ImVec2(pos.x + padding.x, pos.y + (size.y - labelSize.y) * 0.5f);
    ImGui::GetWindowDrawList()->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), label);

    return clicked;
}

bool Selector::render() {

    // lock for streaming stdin
    std::lock_guard<std::mutex> lock(Input::mutex);

    ImGui::Begin("Select",
                 nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_AlwaysAutoResize);

    int clicked = -1;

    if (choices.size() == 0) {
        ImGui::Text("Loading...");
    } else {
        for (int i = 0; i < choices.size(); i++) {
            /*
            if (ImGui::Selectable(choices[i].display.c_str(), selected == i)) {
                selected = i;
                clicked = i;
            }
            */
            bool isSelected = (selected == i);
            if (RoundedSelectableFullWidth(choices[i].display.c_str(), isSelected)) {
                selected = i;
                clicked = i;
            }
        }
        lastSize = ImGui::GetWindowSize();
    }

    ImGui::End();

    if (clicked >= 0) {
        std::cout << choices[clicked].id << std::endl;
        std::cout.flush();
        return false;
    }

    return true;
}
