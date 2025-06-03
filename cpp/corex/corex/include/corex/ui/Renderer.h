#pragma once

#include "Font.h"
#include <cstdint>
#include <string>

namespace ui {

    struct Color {
        uint8_t r, g, b, a;
    };

    class Renderer {
    public:
        static void clear(Color color = {0, 0, 0, 255});
        static void present();

        static void drawText(const std::string& message, const Font& font, int x, int y, Color color);
    };

} // namespace ui
