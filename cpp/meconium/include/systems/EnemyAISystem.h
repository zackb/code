#pragma once

#include "components/Attack.h"
#include "components/Sprite.h"
#include "components/State.h"
#include "components/Transform.h"
#include "components/Velocity.h"
#include "entity/Entity.h"

class EnemyAISystem {
public:
    void update(const std::shared_ptr<Entities>& entities, const int dt) const;

private:
    void performBehavior(
        EnemyAI& ai, Sprite& sprite, State& state, Transform& position, Velocity& velocity, Transform& playerPos) const;

    bool seesTarget(Transform& playerPos, Transform& enemyPos, Attack& attack, bool facingRight) const;
};
