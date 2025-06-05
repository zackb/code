#include "corex/entity/Entity.h"

class InteractionSystem {

public:
    void update(Entities& entities);

private:
    void resolveInteraction(Entities& entities, Entity& player, Entity& interactable) const;
    void resolvePickup(Entities& entities, Entity& player, Entity& pickup) const;
    void resolveDoor(Entities& entities, Entity& player, Entity& door) const;
};
