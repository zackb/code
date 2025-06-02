#pragma once

#include "corex/components/Component.h"
#include <string>

struct SoundEffect : public Component {
    std::string soundId;
    int loops = 0; // 0 = play once
    SoundEffect(std::string soundId, int loops) : soundId(soundId), loops(loops) {}
};
