#pragma once

#include "corex/components/Component.h"
#include <string>

struct OpenDoor : public Component {
    std::string keyId;
    OpenDoor(std::string& keyId) : keyId(keyId) {}
    OpenDoor(std::string&& keyId) : keyId(std::move(keyId)) {}
};

struct MissingKey : public Component {
    std::string message() { return "Missing Key!"; }
};

struct DoorOpened : public Component {};
