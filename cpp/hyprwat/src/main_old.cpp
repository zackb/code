#include "../ext/imgui/backends/imgui_impl_opengl3.h"
#include "../ext/imgui/imgui.h"

extern "C" {
#include "wayland/protocols/wlr-layer-shell-unstable-v1-client-protocol.h"
#include <linux/input-event-codes.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-egl.h>
}

#include <EGL/egl.h>
#include <GL/gl.h>
#include <cstdio>
#include <cstdlib>

struct wl_display* display = nullptr;
struct wl_registry* registry = nullptr;
struct wl_compositor* compositor = nullptr;
struct wl_surface* surface = nullptr;
struct zwlr_layer_shell_v1* layer_shell = nullptr;
struct zwlr_layer_surface_v1* layer_surface = nullptr;
struct wl_seat* seat = nullptr;
struct wl_pointer* pointer = nullptr;
struct wl_keyboard* keyboard = nullptr;
struct wl_egl_window* egl_window = nullptr;
EGLDisplay eglDisplay;
EGLContext eglContext;
EGLSurface eglSurface;

int window_width = 300;
int window_height = 200;
ImGuiIO* io = nullptr;
bool surface_configured = false;
bool should_exit = false;

// ----------------- Pointer Callbacks -----------------
void pointer_enter(
    void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy) {
    io->MousePos = ImVec2((float)wl_fixed_to_int(sx), (float)wl_fixed_to_int(sy));
}

void pointer_leave(void*, wl_pointer*, uint32_t, wl_surface*) {}
void pointer_motion(void* data, wl_pointer*, uint32_t, wl_fixed_t sx, wl_fixed_t sy) {
    io->MousePos = ImVec2((float)wl_fixed_to_int(sx), (float)wl_fixed_to_int(sy));
}
void pointer_button(void*, wl_pointer*, uint32_t, uint32_t, uint32_t button, uint32_t state) {
    if (button == BTN_LEFT) {
        io->MouseDown[0] = (state == WL_POINTER_BUTTON_STATE_PRESSED);

        // Check if click is outside the window bounds
        if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
            if (io->MousePos.x < 0 || io->MousePos.x >= window_width || io->MousePos.y < 0 ||
                io->MousePos.y >= window_height) {
                should_exit = true;
            }
        }
    } else if (button == BTN_RIGHT || button == BTN_MIDDLE) {
        // Right-click or other buttons close the popup
        if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
            should_exit = true;
        }
    }
}
void pointer_axis(void*, wl_pointer*, uint32_t, uint32_t axis, wl_fixed_t value) {
    if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL)
        io->MouseWheel += wl_fixed_to_double(value);
    if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL)
        io->MouseWheelH += wl_fixed_to_double(value);
}
void pointer_frame(void*, wl_pointer*) {}
void pointer_axis_source(void*, wl_pointer*, uint32_t) {}
void pointer_axis_stop(void*, wl_pointer*, uint32_t, uint32_t) {}
void pointer_axis_discrete(void*, wl_pointer*, uint32_t axis, int32_t discrete) {
    if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL)
        io->MouseWheel += (float)discrete;
    if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL)
        io->MouseWheelH += (float)discrete;
}

wl_pointer_listener pointer_listener = {.enter = pointer_enter,
                                        .leave = pointer_leave,
                                        .motion = pointer_motion,
                                        .button = pointer_button,
                                        .axis = pointer_axis,
                                        .frame = pointer_frame,
                                        .axis_source = pointer_axis_source,
                                        .axis_stop = pointer_axis_stop,
                                        .axis_discrete = pointer_axis_discrete};

// ----------------- Keyboard Callbacks -----------------
void keyboard_keymap(void*, wl_keyboard*, uint32_t, int, uint32_t) {}
void keyboard_enter(void*, wl_keyboard*, uint32_t, wl_surface*, wl_array*) {}
void keyboard_leave(void*, wl_keyboard*, uint32_t, wl_surface*) {}
void keyboard_key(void*, wl_keyboard*, uint32_t, uint32_t, uint32_t, uint32_t) {}
void keyboard_modifiers(void*, wl_keyboard*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) {}
void keyboard_repeat_info(void*, wl_keyboard*, int32_t, int32_t) {}

wl_keyboard_listener keyboard_listener = {.keymap = keyboard_keymap,
                                          .enter = keyboard_enter,
                                          .leave = keyboard_leave,
                                          .key = keyboard_key,
                                          .modifiers = keyboard_modifiers,
                                          .repeat_info = keyboard_repeat_info};

// ----------------- Seat Callbacks -----------------
void seat_capabilities(void* data, wl_seat* seat, uint32_t capabilities) {
    if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
        pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(pointer, &pointer_listener, nullptr);
    }
    if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
        keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(keyboard, &keyboard_listener, nullptr);
    }
}

void seat_name(void* data, wl_seat* seat, const char* name) {}

static const wl_seat_listener seat_listener = {.capabilities = seat_capabilities, .name = seat_name};

// ----------------- Layer Surface Callbacks -----------------
void layer_surface_configure(
    void* data, zwlr_layer_surface_v1* layer_surface, uint32_t serial, uint32_t width, uint32_t height) {
    zwlr_layer_surface_v1_ack_configure(layer_surface, serial);
    surface_configured = true;
}

void layer_surface_closed(void* data, zwlr_layer_surface_v1* layer_surface) { exit(0); }

static const zwlr_layer_surface_v1_listener layer_surface_listener = {.configure = layer_surface_configure,
                                                                      .closed = layer_surface_closed};

// ----------------- Registry -----------------
void registry_handler(void*, wl_registry* registry, uint32_t id, const char* interface, uint32_t version) {
    if (strcmp(interface, wl_compositor_interface.name) == 0)
        compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, id, &wl_compositor_interface, 4));
    else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0)
        layer_shell =
            static_cast<zwlr_layer_shell_v1*>(wl_registry_bind(registry, id, &zwlr_layer_shell_v1_interface, 1));
    else if (strcmp(interface, wl_seat_interface.name) == 0) {
        seat = static_cast<wl_seat*>(wl_registry_bind(registry, id, &wl_seat_interface, 5));
        wl_seat_add_listener(seat, &seat_listener, nullptr);
    }
}

void registry_remover(void*, wl_registry*, uint32_t) {}

// ----------------- Create Popup -----------------
void create_popup_surface(int x, int y) {
    surface = wl_compositor_create_surface(compositor);
    layer_surface = zwlr_layer_shell_v1_get_layer_surface(
        layer_shell, surface, nullptr, ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY, "popup_menu");

    zwlr_layer_surface_v1_add_listener(layer_surface, &layer_surface_listener, nullptr);
    zwlr_layer_surface_v1_set_size(layer_surface, window_width, window_height);

    // Anchor to top-left corner of screen, then use margins to position
    zwlr_layer_surface_v1_set_anchor(layer_surface,
                                     ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP | ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT);

    // Set margins to position at (x, y) from top-left
    zwlr_layer_surface_v1_set_margin(layer_surface, y, 0, 0, x);

    zwlr_layer_surface_v1_set_keyboard_interactivity(layer_surface, true);
    zwlr_layer_surface_v1_set_exclusive_zone(layer_surface, 0);

    wl_surface_commit(surface);
}

// ----------------- EGL -----------------
void init_egl() {
    eglDisplay = eglGetDisplay((EGLNativeDisplayType)display);
    eglInitialize(eglDisplay, nullptr, nullptr);

    EGLConfig config;
    EGLint num_config;
    static const EGLint attribs[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE};
    eglChooseConfig(eglDisplay, attribs, &config, 1, &num_config);

    eglBindAPI(EGL_OPENGL_API);
    eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, nullptr);

    egl_window = wl_egl_window_create(surface, window_width, window_height);
    eglSurface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)egl_window, nullptr);
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
}

// ----------------- Main -----------------
int main() {
    display = wl_display_connect(nullptr);
    if (!display) {
        fprintf(stderr, "Failed to connect to Wayland\n");
        return 1;
    }

    registry = wl_display_get_registry(display);
    wl_registry_listener listener = {.global = registry_handler, .global_remove = registry_remover};
    wl_registry_add_listener(registry, &listener, nullptr);
    wl_display_roundtrip(display);

    create_popup_surface(500, 300);

    // Wait for surface to be configured
    while (!surface_configured) {
        wl_display_dispatch(display);
    }

    init_egl();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init("#version 130");

    bool running = true;
    while (running && !should_exit) {
        // Non-blocking event dispatch
        while (wl_display_prepare_read(display) != 0) {
            wl_display_dispatch_pending(display);
        }
        wl_display_flush(display);
        wl_display_read_events(display);
        wl_display_dispatch_pending(display);

        ImGui_ImplOpenGL3_NewFrame();
        io->DisplaySize = ImVec2((float)window_width, (float)window_height);
        io->DeltaTime = 1.0f / 60.0f;

        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)window_width, (float)window_height));
        ImGui::Begin(
            "Popup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::Text("Select an item");
        if (ImGui::Button("Item 1", ImVec2(280, 40))) {
            printf("1\n");
            fflush(stdout);
            running = false;
        }
        if (ImGui::Button("Item 2", ImVec2(280, 40))) {
            printf("2\n");
            fflush(stdout);
            running = false;
        }
        if (ImGui::Button("Cancel", ImVec2(280, 40))) {
            running = false;
        }
        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, window_width, window_height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        eglSwapBuffers(eglDisplay, eglSurface);
    }

    return 0;
}
