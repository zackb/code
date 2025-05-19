#pragma once

#include "SoundManager.h"
#include "components/SoundEffect.h"
#include "entity/Entity.h"
#include <memory>

class SoundSystem {
public:
    void update(const std::shared_ptr<Entities>& entities, SoundManager& soundManager) const {

        auto sounds = entities->findByComponents<SoundEffect>();

        for (auto& entity : sounds) {
            const auto request = entity->getComponent<SoundEffect>();
            soundManager.play(request->soundId, request->loops);
            entities->removeComponent<SoundEffect>(entity);
        }
    }
};
