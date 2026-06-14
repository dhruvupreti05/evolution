#pragma once

#include <string>

#include "entities/direction.h"

/*
    Lists every kind of action an entity can ask to perform.
    The Action struct stores the extra information needed for each type.
*/
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

/*
    Represents one decision made by a brain.
    Some actions use a direction, some use a target tile, and some need neither.
*/
struct Action
{
    ActionType type = ActionType::Stay;
    Direction direction = Direction::Stay;

    // Used by actions that point at a tile, like eating, drinking, attacking, or mating.
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