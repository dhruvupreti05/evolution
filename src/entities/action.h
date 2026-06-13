#pragma once

#include "entities/direction.h"

#include <string>

enum class ActionType
{
    Stay,
    Move,
    Eat,
    Drink,
    Attack,
    PickUp,
    Drop,
    Mate
};

struct Action
{
    ActionType type = ActionType::Stay;
    Direction direction = Direction::Stay;
    int targetX = -1;
    int targetY = -1;

    bool hasTarget() const;
    bool isMovement() const;
    bool isMate() const;
    bool isStay() const;

    std::string toString() const;

    static Action stay();
    static Action move(Direction direction);
    static Action eat(int x, int y);
    static Action drink(int x, int y);
    static Action attack(int x, int y);
    static Action pickUp();
    static Action drop();
    static Action mate(int x, int y);
};