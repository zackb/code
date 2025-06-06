#pragma once

#include "corex/components/Component.h"
#include <string>

struct GrantHealth : public Component {
    int amount;
    GrantHealth(int amt) : amount(amt) {}
};

struct AddToBag : public Component {
    std::string itemId;
    AddToBag(const std::string& iId) : itemId(iId) {}
    AddToBag(std::string&& iId) : itemId(std::move(iId)) {}
};

struct OpenDoor : public Component {
    std::string keyId;
    OpenDoor(std::string& keyId) : keyId(keyId) {}
    OpenDoor(std::string&& keyId) : keyId(std::move(keyId)) {}
};

struct MissingKey : public Component {
    std::string message() { return "Missing Key!"; }
};

struct DoorOpened : public Component {};

struct GoalReached : public Component {};

struct LevelComplete : public Component {};
