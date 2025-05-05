#pragma once
#include <memory>
#include "Position.h"
#include "Velocity.h"
#include "Sprite.h"
#include "InputControl.h"

class Entity {
public:
    int id;
    std::shared_ptr<Position> position;
    std::shared_ptr<Velocity> velocity;
    std::shared_ptr<InputControl> inputControl;
    std::shared_ptr<Sprite> sprite;

    Entity(int id_) : id(id_) {}

    bool hasPosition() { return position != nullptr; }
    bool hasVelocity() { return velocity != nullptr; }
    bool hasInputControl() { return inputControl != nullptr; }
    bool hasSprite() { return sprite != nullptr; }

    void addPosition(int x, int y) { position = std::make_shared<Position>(Position{x, y}); }
    void addVelocity(int vx, int vy) { velocity = std::make_shared<Velocity>(Velocity{vx, vy}); }
    void addInputControl() { inputControl = std::make_shared<InputControl>(); }
    void addSprite(std::shared_ptr<Sprite> s) { sprite = s; }
};
