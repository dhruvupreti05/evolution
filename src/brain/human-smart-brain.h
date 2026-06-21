#pragma once

#include "brain/brain.h"
#include "core/gameworld.h"
#include "entities/action.h"
#include "entities/direction.h"

class Entity;
class Predator;

/*
    Brain used by humans with simple survival logic.
    It chooses between critical water seeking, fleeing predators, mating, finding water, and eating crops.
*/
class HumanSmartBrain : public Brain
{
public:
    Action chooseAction(Entity& entity, GameWorld& world) override;

private:
    Action chooseThirstAction(Entity& entity, GameWorld& world);
    Action chooseFoodAction(Entity& entity, GameWorld& world);
    Action chooseFleeAction(Entity& entity, GameWorld& world, const Predator* predator);

    const Predator* getNearestPredatorWithinRange(Entity& entity, int range);
};