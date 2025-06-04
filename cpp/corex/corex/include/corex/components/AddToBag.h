#pragma once

#include "corex/components/Component.h"
#include <string>

struct AddToBag : public Component {
    std::string itemId;
    AddToBag(const std::string& iId) : itemId(iId) {}
    AddToBag(std::string&& iId) : itemId(std::move(iId)) {}
};
