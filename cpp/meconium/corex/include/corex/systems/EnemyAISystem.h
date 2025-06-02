#pragma once

#include "corex/components/Attack.h"
#include "corex/components/Sprite.h"
#include "corex/components/State.h"
#include "corex/components/Transform.h"
#include "corex/components/Velocity.h"
#include "corex/entity/Entity.h"

class EnemyAISystem {
public:
    void update(const std::shared_ptr<Entities>& entities, const int dt) const;

private:
    void performBehavior(
        EnemyAI& ai, Sprite& sprite, State& state, Transform& position, Velocity& velocity, Transform& playerPos) const;

    bool seesTarget(Transform& playerPos, Transform& enemyPos, Attack& attack, bool facingRight) const;
};
