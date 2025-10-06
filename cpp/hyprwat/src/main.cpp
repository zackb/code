#include "input.hpp"
#include "menu/popup_menu.hpp"
#include "renderer/egl_context.hpp"
#include "selection/selector.hpp"
#include "src/hyprland/ipc.hpp"
#include "ui.hpp"
#include "wayland/display.hpp"
#include "wayland/input.hpp"
#include "wayland/layer_surface.hpp"
#include "wayland/wayland.hpp"

#include "ext/imgui/backends/imgui_impl_opengl3.h"
#include "ext/imgui/imgui.h"

#include <GL/gl.h>
#include <cstdio>

int main(const int argc, const char** argv) {
    int x = 10;
    int y = 10;
    int width = 400;
    int height = 300;

    // Initialize Wayland
    wl::Wayland wayland;

    Selector frame;

    UI ui(wayland);

    // Wait for Wayland outputs to be discovered
    // This ensures we have screen bounds information for clamping
    wayland.display().roundtrip();
    wayland.display().roundtrip(); // Second roundtrip to ensure output events are processed

    // find menu position
    hyprland::Control hyprctl;
    Vec2 pos = hyprctl.getCursorPos();

    // Clamp position to screen bounds to prevent menu going off-screen
    // Convert cursor position to logical coordinates first
    int logicalCursorX = (int)pos.x / wayland.display().getMaxScale();
    int logicalCursorY = (int)pos.y / wayland.display().getMaxScale();
    int menuX = logicalCursorX;
    int menuY = logicalCursorY;
    ui.clampPosition(menuX, menuY, 350, 200); // Conservative estimate

    printf("DEBUG: Creating UI at position=(%d,%d)\n", menuX, menuY);
    
    ui.init(menuX, menuY, width, height);
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

    ui.run(frame);

    return 0;
}

int main1(int argc, char* argv[]) {
    int x = 500;
    int y = 300;
    int width = 300;
    int height = 200;

    // Initialize Wayland
    wl::Wayland wayland;
    wl::Display& display = wayland.display();
    wl::InputHandler& input = wayland.input();

    // Create layer surface
    wl::LayerSurface surface(display.compositor(), display.layerShell());
    surface.create(x, y, width, height);

    // Wait for surface to be configured
    while (!surface.is_configured()) {
        display.dispatch();
    }

    // Initialize EGL
    egl::Context egl(display.display());
    if (!egl.createWindowSurface(surface.surface(), width, height)) {
        return 1;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init("#version 130");

    // Set up input handling
    input.setIO(&io);
    input.setWindowBounds(width, height);

    // Create menu
    PopupMenu menu;
    menu.addItem("Item 1", []() {
        printf("1\n");
        fflush(stdout);
    });
    menu.addItem("Item 2", []() {
        printf("2\n");
        fflush(stdout);
    });

    // Main loop
    bool running = true;
    while (running && !surface.should_exit()) {
        // Process Wayland events
        display.prepareRead();
        display.flush();
        display.readEvents();
        display.dispatchPending();

        // Check if user clicked outside
        if (input.clickedOutside()) {
            running = false;
            break;
        }

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        io.DisplaySize = ImVec2((float)surface.width(), (float)surface.height());
        io.DeltaTime = 1.0f / 60.0f;
        ImGui::NewFrame();

        // Render menu
        if (menu.render(surface.width(), surface.height())) {
            running = false;
        }

        // Render
        ImGui::Render();
        glViewport(0, 0, surface.width(), surface.height());
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        egl.swapBuffers();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
