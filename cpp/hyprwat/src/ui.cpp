#include "ui.hpp"
#include "font.hpp"
#include "signal.hpp"

#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <iostream>

void UI::init(std::string title) {

    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    if (setenv("SDL_VIDEODRIVER", "wayland", 1) != 0) {
        perror("Failed to set SDL_VIDEODRIVER");
    }
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Failed to init SDL with Wayland: %s\n", SDL_GetError());
        // Try fallback
        setenv("SDL_VIDEODRIVER", "x11", 1);
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            fprintf(stderr, "Failed to init SDL with X11 too: %s\n", SDL_GetError());
        }
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // find the cursor position to draw the window at
    int mouseX, mouseY;
    SDL_GetGlobalMouseState(&mouseX, &mouseY);

    int winWidth = 400;
    int winHeight = 200;

    // adjust to keep window fully on-screen
    int screenW, screenH;
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    screenW = dm.w;
    screenH = dm.h;

    int posX = mouseX;
    int posY = mouseY;

    // clamp so window doesn’t go off right or bottom edge
    if (posX + winWidth > screenW)
        posX = screenW - winWidth;
    if (posY + winHeight > screenH)
        posY = screenH - winHeight;

    window = SDL_CreateWindow(title.c_str(),
                              posX,
                              posY,
                              winWidth,
                              winHeight,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALLOW_HIGHDPI);

    if (!window) {
        fprintf(stderr, "Failed to create SDL window: %s\n", SDL_GetError());
        std::exit(1);
    }

    // find dpi
    float ddpi, hdpi, vdpi;
    if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi) == 0) {
        dpi = ddpi / 96.0f;
    }

    // create GL context
    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        fprintf(stderr, "Failed to create OpenGL context: %s\n", SDL_GetError());
        std::exit(1);
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    // ImGui Style
    ImGui::StyleColorsDark();
    auto fontPath = font::defaultFontPath();
    if (!fontPath.empty()) {
        float fontSize = 20.0f;
        ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize * dpi);
        io.FontDefault = font;
    }
    // io.FontGlobalScale = dpi;
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 6.0f;   // Rounded corners for buttons, sliders, etc.
    style.GrabRounding = 6.0f;    // Rounded sliders/knobs
    style.WindowRounding = 10.0f; // Rounded main windows
    style.ChildRounding = 10.0f;  // Rounded child panels
    style.PopupRounding = 8.0f;
    style.FrameBorderSize = 1.0f;
    style.WindowBorderSize = 0.0f;
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(10, 6);
    style.WindowPadding = ImVec2(10, 10);
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.75f); // RGBA

    // init OpenGL backend
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    signalHandler([&](int sig) {
        std::cerr << "caught signal: " << sig << std::endl;
        running = false;
    });
}

void UI::run(Frame& frame) {
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                running = false;
            }
        }
        renderFrame(frame);
    }
}

void UI::resizeWindow(const ImVec2& contentSize) {
    int newWidth = static_cast<int>(contentSize.x * dpi);
    int newHeight = static_cast<int>(contentSize.y * dpi);

    int currentW, currentH;
    SDL_GetWindowSize(window, &currentW, &currentH);

    if (currentW != newWidth || currentH != newHeight) {
        SDL_SetWindowSize(window, newWidth, newHeight);
    }
}

void UI::renderFrame(Frame& frame) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    // ImGuiIO& io = ImGui::GetIO();
    // ImGui::SetNextWindowSize(io.DisplaySize);

    running = frame.render();

    /*
    // After rendering, get the window size safely:
    ImGuiWindow* iwindow = ImGui::GetCurrentContext()->CurrentWindow;
    if (iwindow) {
        ImVec2 contentSize = iwindow->Size;
        resizeWindow(contentSize);
    }
    */

    ImGui::Render();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);
}

void UI::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
