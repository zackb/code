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

    // Pre-calculate desired size based on content
    if (choices.size() > 0) {
        float maxTextWidth = 0;
        float totalHeight = 0;
        ImVec2 padding = ImGui::GetStyle().FramePadding;
        ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;

        for (const auto& choice : choices) {
            ImVec2 textSize = ImGui::CalcTextSize(choice.display.c_str());
            maxTextWidth = std::max(maxTextWidth, textSize.x);
            totalHeight += textSize.y + padding.y * 2;
        }

        // Add some margin and window padding
        float desiredWidth = maxTextWidth + padding.x * 2 + windowPadding.x * 2 + 20; // 20px extra margin
        float desiredHeight = totalHeight + windowPadding.y * 2;

        lastSize = ImVec2(desiredWidth, desiredHeight);
    }

    // Set the window to fill the entire display
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    
    ImGui::Begin("Select",
                 nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoResize);

    int clicked = -1;

    if (choices.size() == 0) {
        ImGui::Text("Loading...");
        lastSize = ImVec2(200, 50); // Fallback size for loading
    } else {
        for (int i = 0; i < choices.size(); i++) {
            bool isSelected = (selected == i);
            if (RoundedSelectableFullWidth(choices[i].display.c_str(), isSelected)) {
                selected = i;
                clicked = i;
            }
        }
        // Update with actual rendered size (for fine-tuning)
        ImVec2 actualSize = ImGui::GetWindowSize();
        if (actualSize.x > lastSize.x * 0.8f && actualSize.y > lastSize.y * 0.8f) {
            lastSize = actualSize;
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
