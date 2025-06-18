#pragma once

#include <raylib.h>

namespace cam {
    void CameraYaw(Camera* camera, float angle, bool rotateAroundTarget);
    void CameraPitch(Camera* camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp);
} // namespace cam
