#pragma once

#include "brain/brain.h"

class Entity;

class RandomBrain : public Brain
{
public:
    Action chooseAction(Entity& entity, GameWorld& world) override;
};