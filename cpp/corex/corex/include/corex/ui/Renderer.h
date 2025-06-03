#pragma once

#include "Font.h"
#include <cstdint>
#include <string>

namespace ui {

    struct Color {
        uint8_t r, g, b, a;
        bool operator==(const Color& other) const {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }
    };

    class Renderer {
    public:
        static void clear(Color color = {0, 0, 0, 255});
        static void present();

        static void drawText(const std::string& message, const Font& font, int x, int y, Color color);
    };

} // namespace ui
