#pragma once

#include "brain/brain.h"

class Entity;

/*
    Brain that makes an entity act randomly.
    Useful for simple movement, testing, or creatures without smarter behavior.
*/
class RandomBrain : public Brain
{
public:
    Action chooseAction(Entity& entity, GameWorld& world) override;
};