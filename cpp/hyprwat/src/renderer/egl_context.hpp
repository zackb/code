#pragma once

extern "C" {
#include <wayland-client.h>
#include <wayland-egl.h>
}

#include <EGL/egl.h>

class zEGLContext {
public:
    zEGLContext(wl_display* display);
    ~zEGLContext();

    bool createWindowSurface(wl_surface* surface, int width, int height);
    void makeCurrent();
    void swapBuffers();
    wl_egl_window* window() const { return egl_window; }

private:
    wl_display* display;
    EGLDisplay egl_display;
    EGLContext egl_context;
    EGLSurface egl_surface;
    wl_egl_window* egl_window = nullptr;
};
