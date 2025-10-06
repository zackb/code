#include "display.hpp"
#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstring>

namespace wl {
    Display::Display() {}

    Display::~Display() {
        for (auto& output : m_outputs) {
            if (output.output)
                wl_output_destroy(output.output);
        }
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
        } else if (strcmp(interface, wl_output_interface.name) == 0) {
            wl_output* output = static_cast<wl_output*>(wl_registry_bind(registry, id, &wl_output_interface, 4));

            static const wl_output_listener output_listener = {.geometry = output_geometry,
                                                               .mode = output_mode,
                                                               .done = output_done,
                                                               .scale = output_scale,
                                                               .name = output_name,
                                                               .description = output_description};
            wl_output_add_listener(output, &output_listener, self);

            self->m_outputs.push_back({output, 1, id});
        }
    }

    int32_t Display::getMaxScale() const {
        int32_t max_scale = 1;
        for (const auto& output : m_outputs) {
            if (output.scale > max_scale) {
                max_scale = output.scale;
            }
        }
        return max_scale;
    }

    void Display::registry_remover(void* data, wl_registry*, uint32_t id) {
        Display* self = static_cast<Display*>(data);

        // Remove output if it was removed
        auto it = std::find_if(
            self->m_outputs.begin(), self->m_outputs.end(), [id](const Output& output) { return output.id == id; });
        if (it != self->m_outputs.end()) {
            if (it->output)
                wl_output_destroy(it->output);
            self->m_outputs.erase(it);

            // Notify about scale change
            if (self->m_scale_callback) {
                self->m_scale_callback(self->getMaxScale());
            }
        }
    }

    void Display::getScreenBounds(int32_t& totalWidth, int32_t& totalHeight) const {
        totalWidth = 0;
        totalHeight = 0;
        
        for (const auto& output : m_outputs) {
            int32_t rightEdge = output.x + output.width;
            int32_t bottomEdge = output.y + output.height;
            
            if (rightEdge > totalWidth) totalWidth = rightEdge;
            if (bottomEdge > totalHeight) totalHeight = bottomEdge;
        }
        
        // Fallback if no outputs detected
        if (totalWidth == 0) totalWidth = 1920;
        if (totalHeight == 0) totalHeight = 1080;
    }

    bool Display::clampToScreen(int32_t& x, int32_t& y, int32_t width, int32_t height) const {
        printf("DEBUG: clampToScreen called with (%d, %d) size %dx%d, outputs: %zu\n", 
               x, y, width, height, m_outputs.size());
               
        if (m_outputs.empty()) {
            printf("DEBUG: No outputs, using fallback 1920x1080\n");
            // Fallback: assume 1920x1080 screen
            x = std::max(0, std::min(x, 1920 - width));
            y = std::max(0, std::min(y, 1080 - height));
            return true;
        }
        
        // Find which output the point (x,y) is on or closest to
        const Output* targetOutput = nullptr;
        int32_t minDistance = INT32_MAX;
        
        for (const auto& output : m_outputs) {
            printf("DEBUG: Output: pos=(%d,%d) size=%dx%d scale=%d\n", 
                   output.x, output.y, output.width, output.height, output.scale);
                   
            // Check if point is within this output
            if (x >= output.x && x < output.x + output.width &&
                y >= output.y && y < output.y + output.height) {
                targetOutput = &output;
                printf("DEBUG: Point is within this output\n");
                break;
            }
            
            // Calculate distance to center of output
            int32_t centerX = output.x + output.width / 2;
            int32_t centerY = output.y + output.height / 2;
            int32_t distance = abs(x - centerX) + abs(y - centerY);
            
            if (distance < minDistance) {
                minDistance = distance;
                targetOutput = &output;
            }
        }
        
        if (targetOutput) {
            // Check if output has valid dimensions
            if (targetOutput->width > 0 && targetOutput->height > 0) {
                printf("DEBUG: Using output bounds: %dx%d at (%d,%d) scale=%d\n", 
                       targetOutput->width, targetOutput->height, targetOutput->x, targetOutput->y, targetOutput->scale);
                       
                // Clamp in physical pixel space (cursor and layer surface positions are in physical pixels)
                // But menu width/height are in logical pixels, so scale them up
                int32_t physicalWidth = width * targetOutput->scale;
                int32_t physicalHeight = height * targetOutput->scale;
                
                printf("DEBUG: Menu size: logical=%dx%d, physical=%dx%d\n", 
                       width, height, physicalWidth, physicalHeight);
                       
                x = std::max(targetOutput->x, std::min(x, targetOutput->x + targetOutput->width - physicalWidth));
                y = std::max(targetOutput->y, std::min(y, targetOutput->y + targetOutput->height - physicalHeight));
                return true;
            } else {
                printf("DEBUG: Output has invalid dimensions, using fallback\n");
                // Output exists but doesn't have valid dimensions yet, use fallback
                x = std::max(0, std::min(x, 1920 - width));
                y = std::max(0, std::min(y, 1080 - height));
                return true;
            }
        }
        
        printf("DEBUG: No suitable output found\n");
        return false;
    }

    // Output event handlers
    void Display::output_geometry(void* data, wl_output* output, int32_t x, int32_t y, int32_t, int32_t, int32_t, const char*, const char*, int32_t) {
        Display* self = static_cast<Display*>(data);
        
        printf("DEBUG: output_geometry called: pos=(%d,%d)\n", x, y);
        
        // Find and update the output position
        for (auto& out : self->m_outputs) {
            if (out.output == output) {
                out.x = x;
                out.y = y;
                break;
            }
        }
    }
    
    void Display::output_mode(void* data, wl_output* output, uint32_t flags, int32_t width, int32_t height, int32_t) {
        // Only update if this is the current mode
        if (flags & WL_OUTPUT_MODE_CURRENT) {
            Display* self = static_cast<Display*>(data);
            
            printf("DEBUG: output_mode called: size=%dx%d\n", width, height);
            
            // Find and update the output size
            for (auto& out : self->m_outputs) {
                if (out.output == output) {
                    out.width = width;
                    out.height = height;
                    break;
                }
            }
        }
    }
    void Display::output_done(void*, wl_output*) {}

    void Display::output_scale(void* data, wl_output* output, int32_t factor) {
        Display* self = static_cast<Display*>(data);

        // Find and update the output scale
        for (auto& out : self->m_outputs) {
            if (out.output == output) {
                out.scale = factor;

                // Notify about scale change
                if (self->m_scale_callback) {
                    self->m_scale_callback(self->getMaxScale());
                }
                break;
            }
        }
    }

    void Display::output_name(void*, wl_output*, const char*) {}
    void Display::output_description(void*, wl_output*, const char*) {}
} // namespace wl
