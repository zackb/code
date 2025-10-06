#pragma once

#include "../renderer/egl_context.hpp"
extern "C" {
#include "protocols/wlr-layer-shell-unstable-v1-client-protocol.h"
#include <wayland-client.h>
}

namespace wl {

    class LayerSurface {
    public:
        LayerSurface(wl_compositor* compositor, zwlr_layer_shell_v1* shell);
        ~LayerSurface();

        void create(int x, int y, int width, int height);
        bool is_configured() const { return m_configured; }
        wl_surface* surface() const { return m_surface; }
        void resize(int new_width, int new_height, egl::Context& egl);

        void request_exit() { m_should_exit = true; }
        bool should_exit() const { return m_should_exit; }

        int width() const { return m_width; }
        int height() const { return m_height; }

        void setBufferScale(int32_t scale);
        int32_t buffer_scale() const { return m_scale; }

    private:
        wl_compositor* m_compositor;
        zwlr_layer_shell_v1* m_layer_shell;
        wl_surface* m_surface = nullptr;
        zwlr_layer_surface_v1* m_layer_surface = nullptr;
        bool m_configured = false;
        bool m_should_exit = false;
        int m_width = 0;
        int m_height = 0;
        int32_t m_scale = 1;

        static void configure_handler(
            void* data, zwlr_layer_surface_v1* layer_surface, uint32_t serial, uint32_t width, uint32_t height);
        static void closed_handler(void* data, zwlr_layer_surface_v1* layer_surface);
    };
} // namespace wl
