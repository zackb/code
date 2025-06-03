#pragma once
#include <string>

namespace ui {
    class Texture {
    public:
        Texture(const std::string& imagePath);
        ~Texture();

        bool isValid() const;
        int width() const;
        int height() const;

        void draw(int x, int y, float scale = 1.0f) const;

    private:
        struct Impl;
        Impl* impl;
    };
} // namespace ui
