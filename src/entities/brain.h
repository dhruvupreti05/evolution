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

class RandomBrain : public Brain
{
public:
    Action chooseAction(Entity& entity, GameWorld& world) override;

private:
    Direction randomDirection() const;
    void getNeighbor(
        int x,
        int y,
        Direction direction,
        int& outX,
        int& outY
    ) const;
};

class ManualBrain : public Brain
{
public:
    Action chooseAction(Entity& entity, GameWorld& world) override;
    void setNextAction(const Action& action);

private:
    Action nextAction = Action::stay();
};

class PredatorSmartBrain : public Brain
{
public:
    Action chooseAction(Entity& entity, GameWorld& world) override;

private:
    Action chooseThirstAction(Entity& entity, GameWorld& world);
    Action chooseHungerAction(Entity& entity, GameWorld& world);

    int manhattanDistance(int x1, int y1, int x2, int y2) const;
    Direction directionToward(
        Entity& entity,
        GameWorld& world,
        int targetX,
        int targetY
    ) const;
    Direction randomDirection() const;
};
