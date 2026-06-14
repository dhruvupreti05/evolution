#pragma once

#include "core/gameworld.h"
#include "entities/action.h"
#include "entities/direction.h"

class Entity;

/*
    Base class for all brain types.
*/
class Brain
{
public:
    virtual ~Brain() = default;

    // Lets each brain type decide what its entity should do next.
    virtual Action chooseAction(Entity& entity, GameWorld& world) = 0;
};