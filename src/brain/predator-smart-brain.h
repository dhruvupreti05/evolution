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
};
