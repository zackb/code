#pragma once
#include <raylib.h>

class Player {
public:
    Player(Vector3 startPos);
    ~Player();

    void Update(float dt, float groundHeight);
    void Draw() const;

    Vector3 GetPosition() const { return position; }
    Vector3 GetForward() const;

    void SetRotation(float angle) { rotation = angle; }
    float GetRotation() const { return rotation; }

private:
    Vector3 position;
    Vector3 velocity;
    float rotation;      // y-axis rotation in radians
    float modelRotation; // the direction the model is facing
    float walkSpeed;
    float runSpeed;
    float jumpForce;
    float gravity;
    bool isGrounded;

    Model model;
    ModelAnimation* anims;
    int animCount;
    int idleAnimIndex;
    int walkAnimIndex;
    int runAnimIndex;
    int jumpAnimIndex;

    int currentAnimIndex;
    int animFrameCounter;

    bool isMoving;
    bool isRunning;
};
