#include "ui.hpp"
#include "font.hpp"
#include "signal.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_video.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <iostream>

void UI::init(std::string title) {
#if not defined(__APPLE__)
    // SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_SCALE_TO_DISPLAY, "1");

    // SDL_SetHint(SDL_HINT_APP_ID, "hyprwat");
    if (setenv("SDL_VIDEODRIVER", "wayland", 1) != 0) {
        perror("Failed to set SDL_VIDEODRIVER");
    }
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Failed to init SDL with Wayland: %s\n", SDL_GetError());
        // Try fallback
        if (setenv("SDL_VIDEODRIVER", "x11", 1) != 0) {
            perror("Failed to set SDL_VIDEODRIVER");
        }
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            fprintf(stderr, "Failed to init SDL with X11 too: %s\n", SDL_GetError());
        }
    }
#else

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Failed to init SDL\n");
        std::exit(1);
    }
#endif

    // Start with a reasonable default size
    int winWidth = 400;
    int winHeight = 200;

    // OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(title.c_str(),
                              winWidth,
                              winHeight,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP |
                                  SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
    if (!window) {
        fprintf(stderr, "Failed to create SDL window\n");
        std::exit(1);
    }

    SDL_SetWindowMinimumSize(window, 0, 0);

    // Position window under cursor
    float mouseX, mouseY;
    SDL_GetGlobalMouseState(&mouseX, &mouseY);
    SDL_SetWindowPosition(window, int(mouseX), int(mouseY));

    // Create OpenGL context
    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        fprintf(stderr, "Failed to create OpenGL context\n");
        std::exit(1);
    }
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    float scale = SDL_GetWindowDisplayScale(window);
    io.DisplayFramebufferScale = ImVec2(scale, scale);
    fprintf(stderr, "Display scale: %f\n", scale);

    auto fontPath = font::defaultFontPath();
    if (!fontPath.empty()) {
        ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 14.0f * scale);
        io.FontDefault = font;
    }

    io.FontGlobalScale = 1.0f / scale;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.FrameRounding = 6.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(10, 6);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.75f);

    // Initialize ImGui backends
    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 150");

    signalHandler([&](int sig) {
        std::cerr << "caught signal: " << sig << std::endl;
        running = false;
    });
}

void UI::run(Frame& frame) {
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                running = false;
                break;
            default:
                break;
            }
        }
        renderFrame(frame);
    }
}

void UI::renderFrame(Frame& frame) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    // ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    running = frame.render();

    Vec2 windowSize = frame.getSize();

    ImGui::Render();

    // TODO: this works
    // SDL_SetWindowSize(window, 298, 92);

    static bool hasResized = false;
    // Resize SDL window to match content (only if size changed)
    if (!hasResized && windowSize.x > 200.0) {
        std::cerr << "Resizing SDL window to: " << windowSize.x << "x" << windowSize.y << std::endl;
        // TODO: this does not "stick"
        SDL_SetWindowSize(window, (int)windowSize.x, (int)windowSize.y);
        SDL_ShowWindow(window);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        std::cerr << "SDL window size after resize: " << w << "x" << h << std::endl;
        hasResized = true;
    }

    int w, h;
    // SDL_GetWindowSizeInPixels(window, &w, &h);
    SDL_GetWindowSize(window, &w, &h);
    glViewport(0, 0, w, h);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

void UI::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
