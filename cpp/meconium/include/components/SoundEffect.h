#pragma once

#include "components/Component.h"
#include <string>

struct SoundEffect : public Component {
    std::string soundId;
    int loops = 0; // 0 = play once
};
