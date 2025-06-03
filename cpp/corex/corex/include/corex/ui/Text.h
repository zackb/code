#pragma once

#include "corex/ui/Font.h"
#include "corex/ui/Renderer.h"
#include <string>

namespace ui {

    class Text {
    public:
        Text(const std::string& message, const Font& font, const Color& color);
        ~Text();

        int width() const;
        int height() const;

        void draw(int x, int y) const;

    private:
        struct Impl;
        Impl* impl;
    };
} // namespace ui
