#pragma once

struct Size {
    int width;
    int height;

    Size() = default;
    Size(int w, int h) : width(w), height(h) {}
};