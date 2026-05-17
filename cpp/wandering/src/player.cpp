#include "player.hpp"
#include "debug.hpp"
#include <cstring>
#include <iostream>
#include <raymath.h>

Player::Player(Vector3 startPos)
    : position(startPos)
    , velocity({0, 0, 0})
    , rotation(0.0f)
    , modelRotation(0.0f)
    , walkSpeed(10.0f)
    , runSpeed(20.0f)
    , jumpForce(12.0f)
    , gravity(30.0f)
    , isGrounded(false)
    , animCount(0)
    , idleAnimIndex(0)
    , walkAnimIndex(0)
    , runAnimIndex(0)
    , jumpAnimIndex(0)
    , currentAnimIndex(0)
    , animFrameCounter(0)
    , isMoving(false)
    , isRunning(false) {

    // load the glb model
    model = LoadModel("assets/zlorp.glb");

    // load the animations from the glb file
    anims = LoadModelAnimations("assets/zlorp.glb", &animCount);

    std::cout << "Loaded " << animCount << " animations:" << std::endl;

    for (int i = 0; i < animCount; i++) {
        std::cout << "Anim " << i << ": '" << anims[i].name << "' (" << anims[i].keyframeCount << " frames)"
                  << std::endl;

        if (strstr(anims[i].name, "Taunt") != nullptr)
            idleAnimIndex = i;
        else if (strstr(anims[i].name, "Walk") != nullptr)
            walkAnimIndex = i;
        else if (strstr(anims[i].name, "Run") != nullptr)
            runAnimIndex = i;
        else if (strstr(anims[i].name, "Jump") != nullptr)
            jumpAnimIndex = i;
    }

    currentAnimIndex = idleAnimIndex;
}

Player::~Player() {
    UnloadModel(model);
    if (anims != nullptr) {
        UnloadModelAnimations(anims, animCount);
    }
}

void Player::Update(float dt, float groundHeight) {
    Vector2 input = {0.0f, 0.0f};
    if (IsKeyDown(KEY_W))
        input.y += 1.0f;
    if (IsKeyDown(KEY_S))
        input.y -= 1.0f;
    if (IsKeyDown(KEY_A))
        input.x -= 1.0f;
    if (IsKeyDown(KEY_D))
        input.x += 1.0f;

    if (Vector2Length(input) > 0.0f) {
        input = Vector2Normalize(input);
        isMoving = true;
        isRunning = IsKeyDown(KEY_LEFT_SHIFT);
    } else {
        isMoving = false;
        isRunning = false;
    }

    // move forward based on player's rotation (set by camera)
    Vector3 forward = {cosf(rotation), 0.0f, -sinf(rotation)};
    Vector3 right = {sinf(rotation), 0.0f, cosf(rotation)};

    Vector3 moveDir = {0.0f, 0.0f, 0.0f};
    if (isMoving) {
        moveDir = Vector3Add(Vector3Scale(forward, input.y), Vector3Scale(right, input.x));
        moveDir = Vector3Normalize(moveDir);

        // calculate target rotation to face movement direction
        float targetRotation = atan2f(-moveDir.z, moveDir.x);

        // smoothly rotate character to face target
        float diff = targetRotation - modelRotation;
        while (diff < -PI)
            diff += 2.0f * PI;
        while (diff > PI)
            diff -= 2.0f * PI;
        modelRotation += diff * 15.0f * dt;

        float currentSpeed = isRunning ? runSpeed : walkSpeed;
        position.x += moveDir.x * currentSpeed * dt;
        position.z += moveDir.z * currentSpeed * dt;
    }

    // jumping & gravity
    if (isGrounded && IsKeyPressed(KEY_SPACE)) {
        velocity.y = jumpForce;
        isGrounded = false;
    }

    velocity.y -= gravity * dt;
    position.y += velocity.y * dt;

    // terrain collision
    // assume player is height 2, origin at center.
    bool wasGrounded = isGrounded;

    if (position.y - 1.0f <= groundHeight) {
        position.y = groundHeight + 1.0f;
        velocity.y = 0.0f;
        isGrounded = true;
    } else {
        // downhill ground snapping, prefent micro-falling jitter when walking down slopes.
        // if we were grounded last frame, aren't jumping upwards, and the ground
        // is within a short distance below us, snap to it.
        float snapDistance = 0.5f;
        if (wasGrounded && velocity.y <= 0.0f && (position.y - 1.0f) <= groundHeight + snapDistance) {
            position.y = groundHeight + 1.0f;
            velocity.y = 0.0f;
            isGrounded = true;
        } else {
            isGrounded = false;
        }
    }

    // update animation
    int nextAnimIndex = currentAnimIndex;

    if (!isGrounded) {
        nextAnimIndex = jumpAnimIndex;
    } else {
        if (isMoving) {
            nextAnimIndex = isRunning ? runAnimIndex : walkAnimIndex;
        } else {
            nextAnimIndex = idleAnimIndex;
        }
    }

    if (nextAnimIndex != currentAnimIndex) {
        currentAnimIndex = nextAnimIndex;
        animFrameCounter = 0;
    }

    if (animCount > 0) {
        animFrameCounter++;
        UpdateModelAnimation(model, anims[currentAnimIndex], animFrameCounter);

        // hold the last frame of the jump animation while falling
        if (currentAnimIndex == jumpAnimIndex) {
            if (animFrameCounter >= anims[currentAnimIndex].keyframeCount - 1) {
                animFrameCounter = anims[currentAnimIndex].keyframeCount - 1;
            }
        } else {
            if (animFrameCounter >= anims[currentAnimIndex].keyframeCount) {
                animFrameCounter = 0;
            }
        }
    }
}

void Player::Draw() const {
    // models usually have their origin at the feet, so we shift down by 1.0f.
    Vector3 drawPos = position;
    drawPos.y -= 1.0f;
    float drawRot = (modelRotation * RAD2DEG) + 90.0f;
    DrawModelEx(model, drawPos, {0.0f, 1.0f, 0.0f}, drawRot, {1.0f, 1.0f, 1.0f}, WHITE);

    if (debug::enabled) {
        // debug cube at the player's center so we can always see where they are
        DrawCubeWires(position, 1.0f, 2.0f, 1.0f, RED);
    }
}

Vector3 Player::GetForward() const { return {cosf(rotation), 0.0f, -sinf(rotation)}; }
