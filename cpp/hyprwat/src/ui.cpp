#include "ui.hpp"
#include "imgui_impl_opengl3.h"
#include "src/font/font.hpp"
#include <GL/gl.h>

void UI::init(int x, int y, int width, int height) {

    // Create wl layer surface
    surface = std::make_unique<wl::LayerSurface>(wayland.display().compositor(), wayland.display().layerShell());
    surface->create(x, y, width, height);
    while (!surface->is_configured()) {
        wayland.display().dispatch();
    }

    // Initialize EGL
    egl = std::make_unique<egl::Context>(wayland.display().display());
    if (!egl->createWindowSurface(surface->surface(), surface->width(), surface->height())) {
        throw std::runtime_error("Failed to create EGL window surface");
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplOpenGL3_Init("#version 130");
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.DisplaySize = ImVec2((float)surface->width(), (float)surface->height());

    io.DisplayFramebufferScale = ImVec2(1.0, 1.0);

    // load user font if available
    auto fontPath = font::defaultFontPath();
    if (!fontPath.empty()) {
        ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 14.0f);
        io.FontDefault = font;
    }

    // Set up our ImGui style
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.ItemSpacing = ImVec2(10, 6);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.75f);

    style.WindowRounding = 10.0f;
    style.FrameRounding = 6.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(8, 4);

    // Set up input handling wayland -> imgui
    wayland.input().setIO(&io);
    wayland.input().setWindowBounds(width, height);
}

void UI::run(Frame& frame) {
    while (running && !surface->should_exit()) {
        // Process Wayland events
        wayland.display().prepareRead();
        wayland.display().flush();
        wayland.display().readEvents();
        wayland.display().dispatchPending();

        // Check if user clicked outside
        if (wayland.input().clickedOutside()) {
            running = false;
            break;
        }

        // Render ImGui frame
        renderFrame(frame);
    }
}

void UI::renderFrame(Frame& frame) {

    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = 1.0f / 60.0f;
    io.DisplaySize = ImVec2((float)surface->width(), (float)surface->height());

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    // ImGui::SetNextWindowSize(ImVec2(surface->width(), surface->height()));

    running = frame.render();
    Vec2 windowSize = frame.getSize();

    // Render
    ImGui::Render();

    static Vec2 lastWindowSize;
    if (windowSize != lastWindowSize) {
        surface->resize((int)windowSize.x, (int)windowSize.y, *egl);
        wayland.input().setWindowBounds((int)windowSize.x, (int)windowSize.y);
    }

    glViewport(0, 0, surface->width(), surface->height());
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // EGL buffer swap
    egl->swapBuffers();
}
