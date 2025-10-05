#include "egl_context.hpp"
#include <cstdio>

namespace egl {

    Context::Context(wl_display* display) : display(display) {
        egl_display = eglGetDisplay((EGLNativeDisplayType)display);
        if (egl_display == EGL_NO_DISPLAY) {
            fprintf(stderr, "Failed to get EGL display\n");
            return;
        }

        if (!eglInitialize(egl_display, nullptr, nullptr)) {
            fprintf(stderr, "Failed to initialize EGL\n");
            return;
        }

        EGLConfig config;
        EGLint num_config;
        static const EGLint attribs[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE};
        eglChooseConfig(egl_display, attribs, &config, 1, &num_config);

        eglBindAPI(EGL_OPENGL_API);
        egl_context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, nullptr);
        if (egl_context == EGL_NO_CONTEXT) {
            fprintf(stderr, "Failed to create EGL context\n");
            return;
        }

        egl_surface = EGL_NO_SURFACE;
    }

    Context::~Context() {
        if (egl_window)
            wl_egl_window_destroy(egl_window);
        if (egl_surface != EGL_NO_SURFACE)
            eglDestroySurface(egl_display, egl_surface);
        if (egl_context != EGL_NO_CONTEXT)
            eglDestroyContext(egl_display, egl_context);
        if (egl_display != EGL_NO_DISPLAY)
            eglTerminate(egl_display);
    }

    bool Context::createWindowSurface(wl_surface* surface, int width, int height) {
        egl_window = wl_egl_window_create(surface, width, height);
        if (!egl_window) {
            fprintf(stderr, "Failed to create EGL window\n");
            return false;
        }

        EGLConfig config;
        EGLint num_config;
        static const EGLint attribs[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE};
        eglChooseConfig(egl_display, attribs, &config, 1, &num_config);

        egl_surface = eglCreateWindowSurface(egl_display, config, (EGLNativeWindowType)egl_window, nullptr);
        if (egl_surface == EGL_NO_SURFACE) {
            fprintf(stderr, "Failed to create EGL surface\n");
            return false;
        }

        makeCurrent();
        return true;
    }

    void Context::makeCurrent() { eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context); }

    void Context::swapBuffers() { eglSwapBuffers(egl_display, egl_surface); }
} // namespace egl
