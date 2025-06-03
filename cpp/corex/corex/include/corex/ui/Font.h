#pragma once

#include <string>

namespace ui {
    class Font {
    public:
        Font(const std::string& path, int size);
        ~Font();

        bool isValid() const;

        void* getSDLFont() const;

    private:
        struct Impl;
        Impl* impl;
    };
} // namespace ui
