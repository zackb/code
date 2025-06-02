#pragma once

#include <cmath>

struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}

    // Addition
    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }

    // Subtraction
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }

    // Scalar multiplication
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }

    // Scalar division
    Vec2 operator/(float scalar) const { return Vec2(x / scalar, y / scalar); }

    // Compound assignment
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    // Length / magnitude
    float length() const { return std::sqrt(x * x + y * y); }

    // Normalize vector (make length = 1)
    Vec2 normalized() const {
        float len = length();
        if (len == 0)
            return Vec2(0, 0);
        return Vec2(x / len, y / len);
    }
};
