#include "display.hpp"
#include <cstdio>
#include <cstring>

namespace wl {
    Display::Display() {}

    Display::~Display() {
        if (m_seat)
            wl_seat_destroy(m_seat);
        if (m_layer_shell)
            zwlr_layer_shell_v1_destroy(m_layer_shell);
        if (m_compositor)
            wl_compositor_destroy(m_compositor);
        if (m_registry)
            wl_registry_destroy(m_registry);
        if (m_display)
            wl_display_disconnect(m_display);
    }

    bool Display::connect() {
        m_display = wl_display_connect(nullptr);
        if (!m_display) {
            fprintf(stderr, "Failed to connect to Wayland display\n");
            return false;
        }

        m_registry = wl_display_get_registry(m_display);
        wl_registry_listener listener = {.global = registry_handler, .global_remove = registry_remover};
        wl_registry_add_listener(m_registry, &listener, this);
        wl_display_roundtrip(m_display);

        if (!m_compositor || !m_layer_shell) {
            fprintf(stderr, "Compositor or layer shell not available\n");
            return false;
        }

        return true;
    }

    void Display::dispatch() { wl_display_dispatch(m_display); }

    void Display::dispatchPending() { wl_display_dispatch_pending(m_display); }

    void Display::roundtrip() { wl_display_roundtrip(m_display); }

    void Display::prepareRead() {
        while (wl_display_prepare_read(m_display) != 0) {
            wl_display_dispatch_pending(m_display);
        }
    }

    void Display::readEvents() { wl_display_read_events(m_display); }

    void Display::flush() { wl_display_flush(m_display); }

    void Display::registry_handler(
        void* data, wl_registry* registry, uint32_t id, const char* interface, uint32_t version) {
        Display* self = static_cast<Display*>(data);

        if (strcmp(interface, wl_compositor_interface.name) == 0) {
            self->m_compositor =
                static_cast<wl_compositor*>(wl_registry_bind(registry, id, &wl_compositor_interface, 4));
        } else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
            self->m_layer_shell =
                static_cast<zwlr_layer_shell_v1*>(wl_registry_bind(registry, id, &zwlr_layer_shell_v1_interface, 1));
        } else if (strcmp(interface, wl_seat_interface.name) == 0) {
            self->m_seat = static_cast<wl_seat*>(wl_registry_bind(registry, id, &wl_seat_interface, 5));
        }
    }

    void Display::registry_remover(void*, wl_registry*, uint32_t) {}
} // namespace wl
