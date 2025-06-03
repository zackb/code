#pragma once

#include "corex/ui/Font.h"
#include "corex/ui/Renderer.h"
#include <string>

namespace ui {

    class Text {
    public:
        Text(const std::string& message, const Font& font, const Color& color);
        ~Text();

        // Rule of 5: explicitly default move, delete copy
        Text(Text&&) noexcept;
        Text& operator=(Text&&) noexcept;

        Text(const Text&) = delete;
        Text& operator=(const Text&) = delete;

        int width() const;
        int height() const;

        void draw(int x, int y) const;
        void draw(int x, int y, const Color& color) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };
} // namespace ui
