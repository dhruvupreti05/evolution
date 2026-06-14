#include "brain/brain.h"
#include "brain/manual-brain.h"

#include "core/config.h"
#include "entities/entity.h"
#include "entities/human.h"
#include "entities/crop.h"

#include <cstdlib>
#include <cmath>

/*
    Returns the action that was manually queued by the player.
    After using it once, the brain resets back to staying still.
*/
Action ManualBrain::chooseAction(Entity& entity, GameWorld& world)
{
    Action action = nextAction;

    // Prevents the same manual input from repeating every update.
    nextAction = Action::stay();

    return action;
}

/*
    Stores the next action that this brain should perform.
    This is usually called when the player presses a manual control key.
*/
void ManualBrain::setNextAction(const Action& action)
{
    nextAction = action;
}