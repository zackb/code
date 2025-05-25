#include "entity/Entity.h"

class PickupSystem {

public:
    void update(Entities& entities);

private:
    void resolvePlayerPickupCollisions(Entity& player, Entity& pickup) const;
};
