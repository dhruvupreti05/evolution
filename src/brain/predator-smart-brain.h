#pragma once

#include "brain/brain.h"
#include "core/gameworld.h"
#include "entities/action.h"
#include "entities/direction.h"

class Entity;


class PredatorSmartBrain : public Brain
{
public:
    Action chooseAction(Entity& entity, GameWorld& world) override;

private:
    Action chooseThirstAction(Entity& entity, GameWorld& world);
    Action chooseHungerAction(Entity& entity, GameWorld& world);

    int manhattanDistance(int x1, int y1, int x2, int y2) const;
    Direction directionToward(
        Entity& entity,
        GameWorld& world,
        int targetX,
        int targetY
    ) const;
    Direction randomDirection() const;
};
