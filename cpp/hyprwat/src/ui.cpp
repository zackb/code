#include "ui.hpp"
#include "imgui_impl_opengl3.h"
#include "src/font/font.hpp"
#include <GL/gl.h>

void UI::init(int x, int y, int width, int height) {

    // Create wl layer surface with a reasonable initial size
    // This will be resized based on content, but avoids starting too large
    int initialWidth = std::min(width, 400);   // Cap initial width
    int initialHeight = std::min(height, 300); // Cap initial height

    surface = std::make_unique<wl::LayerSurface>(wayland.display().compositor(), wayland.display().layerShell());
    surface->create(x, y, initialWidth, initialHeight);
    while (!surface->is_configured()) {
        wayland.display().dispatch();
    }

    // Get the current maximum scale from all outputs
    current_scale = wayland.display().getMaxScale();

    // Set up callback for dynamic scale changes
    wayland.display().setScaleChangeCallback([this](int32_t new_scale) { updateScale(new_scale); });

    // Apply buffer scale (HiDPI). Keep logical size for layer surface sizing,
    // but render buffers (EGL window) in pixel size.
    surface->setBufferScale(current_scale);

    // Initialize EGL
    egl = std::make_unique<egl::Context>(wayland.display().display());
    // Create EGL window with buffer pixel size (logical * buffer_scale)
    const int buf_w = surface->width() * current_scale;
    const int buf_h = surface->height() * current_scale;
    if (!egl->createWindowSurface(surface->surface(), buf_w, buf_h)) {
        throw std::runtime_error("Failed to create EGL window surface");
    }

    // enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplOpenGL3_Init("#version 100");
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    // Logical size in points
    io.DisplaySize = ImVec2((float)surface->width(), (float)surface->height());

    // Framebuffer scale = buffer pixels / logical points
    io.DisplayFramebufferScale = ImVec2((float)current_scale, (float)current_scale);

    // load user font if available
    auto fontPath = font::defaultFontPath();
    if (!fontPath.empty()) {
        ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 14.0f);
        io.FontDefault = font;
    }
    // Use 1.0f font global scale; HiDPI handled by DisplayFramebufferScale
    io.FontGlobalScale = 1.0f;

    // Set up our ImGui style
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    // style.ScaleAllSizes(scale);

    style.ItemSpacing = ImVec2(10, 6);
    // style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f); -- no transparent
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);

    style.WindowRounding = 10.0f;
    style.FrameRounding = 6.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(8, 4);

    // Set up input handling wayland -> imgui
    wayland.input().setIO(&io);
    // Input bounds in logical units
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
    io.DisplayFramebufferScale = ImVec2((float)current_scale, (float)current_scale);

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    running = frame.render();
    Vec2 desiredSize = frame.getSize();

    // Render (but don't swap yet)
    ImGui::Render();

    static Vec2 lastWindowSize;
    static int resizeStabilityCounter = 0;
    const int RESIZE_STABILITY_FRAMES = 3; // Wait 3 frames before resizing

    // Check if size changed
    if (desiredSize != lastWindowSize) {
        resizeStabilityCounter = 0; // Reset counter on size change
        lastWindowSize = desiredSize;
    } else {
        resizeStabilityCounter++;
    }

    // Only resize after size has been stable for a few frames
    if (resizeStabilityCounter == RESIZE_STABILITY_FRAMES &&
        (desiredSize.x != surface->width() || desiredSize.y != surface->height())) {

        // Clamp to reasonable bounds
        int newWidth = std::max(100, (int)desiredSize.x);
        int newHeight = std::max(50, (int)desiredSize.y);

        surface->resize(newWidth, newHeight, *egl);
        wayland.input().setWindowBounds(newWidth, newHeight);

        // Update display size for next frame
        io.DisplaySize = ImVec2((float)newWidth, (float)newHeight);
    }

    // Use buffer pixel size for viewport
    Vec2 bufSize = egl->getBufferSize();
    glViewport(0, 0, (int)bufSize.x, (int)bufSize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // EGL buffer swap
    egl->swapBuffers();
}

void UI::updateScale(int32_t new_scale) {
    if (new_scale == current_scale || !surface || !egl) {
        return;
    }

    current_scale = new_scale;

    // Update buffer scale
    surface->setBufferScale(current_scale);

    // Resize EGL window with new buffer size
    const int buf_w = surface->width() * current_scale;
    const int buf_h = surface->height() * current_scale;
    if (egl->window()) {
        wl_egl_window_resize(egl->window(), buf_w, buf_h, 0, 0);
    }

    // ImGui will be updated in the next renderFrame() call
}
