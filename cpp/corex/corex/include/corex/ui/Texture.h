#pragma once
#include <string>

namespace ui {
    class Texture {
    public:
        explicit Texture(const std::string& imagePath);
        ~Texture();
        Texture(Texture&&) noexcept;
        Texture& operator=(Texture&&) noexcept;

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        bool isValid() const;
        int width() const;
        int height() const;

        void draw(int x, int y, float scale = 1.0f) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };
} // namespace ui
