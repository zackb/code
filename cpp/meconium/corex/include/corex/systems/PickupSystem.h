#include "corex/entity/Entity.h"

class PickupSystem {

public:
    void update(Entities& entities);

private:
    void resolvePlayerPickupCollisions(Entities& entities, Entity& player, Entity& pickup) const;
};
