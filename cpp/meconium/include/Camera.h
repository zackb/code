#pragma once

class Camera {
    public:
        int x, y;  // Camera position

        static Camera& getInstance() {
            static Camera instance;
            return instance;
        }
    
    private:
        Camera() : x(0), y(0) {}
        Camera(const Camera&) = delete;  // Prevent copy constructor
        Camera& operator=(const Camera&) = delete;  // Prevent assignment
    };