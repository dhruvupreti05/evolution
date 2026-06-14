#pragma once

#include "brain/brain.h"
#include "core/gameworld.h"
#include "entities/action.h"
#include "entities/direction.h"

class Entity;

/*
    Brain used by predators with simple survival logic.
    It chooses between mating, finding water, and hunting humans.
*/
class PredatorSmartBrain : public Brain
{
public:
    Action chooseAction(Entity& entity, GameWorld& world) override;

private:
    // Handles predator behavior when thirst is the biggest problem.
    Action chooseThirstAction(Entity& entity, GameWorld& world);

    // Handles predator behavior when it is ready to hunt or eat.
    Action chooseHungerAction(Entity& entity, GameWorld& world);
};