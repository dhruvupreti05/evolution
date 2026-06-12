#pragma once

#include "brain/brain.h"
#include "core/gameworld.h"
#include "entities/action.h"
#include "entities/direction.h"

class Entity;

class ManualBrain : public Brain
{
public:
    Action chooseAction(Entity& entity, GameWorld& world) override;
    void setNextAction(const Action& action);

private:
    Action nextAction = Action::stay();
};