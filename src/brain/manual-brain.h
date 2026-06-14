#pragma once

#include "brain/brain.h"
#include "core/gameworld.h"
#include "entities/action.h"
#include "entities/direction.h"

class Entity;

/*
    Brain used when the player controls an entity directly.
    It stores one queued action at a time, then clears it after the action is used.
*/
class ManualBrain : public Brain
{
public:
    Action chooseAction(Entity& entity, GameWorld& world) override;
    void setNextAction(const Action& action);

private:
    // Holds the player's next command, and starts as stay so the entity does nothing by default.
    Action nextAction = Action::stay();
};