#pragma once

#include "core/gameworld.h"
#include "entities/action.h"
#include "entities/direction.h"

class Entity;

class Brain
{
public:
    virtual ~Brain() = default;
    virtual Action chooseAction(Entity& entity, GameWorld& world) = 0;
};
