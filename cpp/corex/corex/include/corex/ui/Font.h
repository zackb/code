#pragma once

#include <string>

namespace ui {
    class Font {
    public:
        Font(const std::string& path, int size);
        ~Font();

        Font(Font&&) noexcept;
        Font& operator=(Font&&) noexcept;

        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;

        bool isValid() const;

        void* getSDLFont() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };
} // namespace ui
