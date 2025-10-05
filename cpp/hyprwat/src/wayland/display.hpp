#pragma once

extern "C" {
#include "protocols/wlr-layer-shell-unstable-v1-client-protocol.h"
#include <wayland-client.h>
}

class WaylandDisplay {
public:
    WaylandDisplay();
    ~WaylandDisplay();

    bool connect();
    void dispatch();
    void dispatchPending();
    void roundtrip();
    void prepareRead();
    void readEvents();
    void flush();

    wl_display* display() const { return m_display; }
    wl_compositor* compositor() const { return m_compositor; }
    zwlr_layer_shell_v1* layerShell() const { return m_layer_shell; }
    wl_seat* seat() const { return m_seat; }

private:
    wl_display* m_display = nullptr;
    wl_registry* m_registry = nullptr;
    wl_compositor* m_compositor = nullptr;
    zwlr_layer_shell_v1* m_layer_shell = nullptr;
    wl_seat* m_seat = nullptr;

    static void
        registry_handler(void* data, wl_registry* registry, uint32_t id, const char* interface, uint32_t version);
    static void registry_remover(void* data, wl_registry* registry, uint32_t id);
};
