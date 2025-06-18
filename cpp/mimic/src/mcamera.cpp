#include "mcamera.hpp"
#include <raymath.h>

namespace cam {

    Vector3 GetCameraForward(Camera* camera) {
        return Vector3Normalize(Vector3Subtract(camera->target, camera->position));
    }

    Vector3 GetCameraUp(Camera* camera) { return Vector3Normalize(camera->up); }

    Vector3 GetCameraRight(Camera* camera) {
        Vector3 forward = GetCameraForward(camera);
        Vector3 up = GetCameraUp(camera);

        return Vector3Normalize(Vector3CrossProduct(forward, up));
    }

    void CameraYaw(Camera* camera, float angle, bool rotateAroundTarget) {
        Matrix rotation = MatrixRotate(camera->up, angle);
        Vector3 target = rotateAroundTarget ? camera->target : Vector3Zero();
        Vector3 camPosition = Vector3Subtract(camera->position, target);
        camPosition = Vector3Transform(camPosition, rotation);
        camera->position = Vector3Add(target, camPosition);
    }

    void CameraPitch(Camera* camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp) {
        // Up direction
        Vector3 up = GetCameraUp(camera);

        // View vector
        Vector3 targetPosition = Vector3Subtract(camera->target, camera->position);

        if (lockView) {
            // In these camera modes we clamp the Pitch angle
            // to allow only viewing straight up or down.

            // Clamp view up
            float maxAngleUp = Vector3Angle(up, targetPosition);
            maxAngleUp -= 0.001f; // avoid numerical errors
            if (angle > maxAngleUp)
                angle = maxAngleUp;

            // Clamp view down
            float maxAngleDown = Vector3Angle(Vector3Negate(up), targetPosition);
            maxAngleDown *= -1.0f;  // downwards angle is negative
            maxAngleDown += 0.001f; // avoid numerical errors
            if (angle < maxAngleDown)
                angle = maxAngleDown;
        }

        // Rotation axis
        Vector3 right = GetCameraRight(camera);

        // Rotate view vector around right axis
        targetPosition = Vector3RotateByAxisAngle(targetPosition, right, angle);

        if (rotateAroundTarget) {
            // Move position relative to target
            camera->position = Vector3Subtract(camera->target, targetPosition);
        } else // rotate around camera.position
        {
            // Move target relative to position
            camera->target = Vector3Add(camera->position, targetPosition);
        }

        if (rotateUp) {
            // Rotate up direction around right axis
            camera->up = Vector3RotateByAxisAngle(camera->up, right, angle);
        }
    }

} // namespace cam
