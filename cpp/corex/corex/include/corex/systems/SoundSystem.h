#pragma once

#include "corex/SoundManager.h"
#include "corex/components/SoundEffect.h"
#include "corex/entity/Entity.h"

class SoundSystem {

public:
    void update(Entities& entities, SoundManager& soundManager) const {

        auto sounds = entities.filtered<SoundEffect>();

        for (auto& entity : sounds) {
            const auto sound = entity->getComponent<SoundEffect>();
            soundManager.play(sound->soundId, sound->loops);
            entities.removeComponent<SoundEffect>(entity);
        }
    }
};
