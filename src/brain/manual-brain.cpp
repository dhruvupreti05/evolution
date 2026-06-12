#include "brain/brain.h"
#include "brain/manual-brain.h"

#include "core/config.h"
#include "entities/entity.h"
#include "entities/human.h"
#include "entities/crop.h"

#include <cstdlib>
#include <cmath>


Action ManualBrain::chooseAction(Entity& entity, GameWorld& world)
{
    Action action = nextAction;
    nextAction = Action::stay();
    return action;
}

void ManualBrain::setNextAction(const Action& action)
{
    nextAction = action;
}