#pragma once

#include "SoundManager.h"
#include "components/SoundEffect.h"
#include "entity/Entity.h"

class SoundSystem {
public:
    void update(Entities& entities, SoundManager& soundManager) const {

        auto sounds = entities.findByComponents<SoundEffect>();

        for (auto& entity : sounds) {
            const auto sound = entity->getComponent<SoundEffect>();
            soundManager.play(sound->soundId, sound->loops);
            entities.removeComponent<SoundEffect>(entity);
        }
    }
};
