#pragma once

struct Rect {
    int x;
    int y;
    int width;
    int height;

public:
    Rect() {}
    Rect(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}
};
