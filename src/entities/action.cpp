#include "entities/action.h"

#include <string>

namespace
{
    /*
        Converts a direction into readable text for debug logs.
    */
    std::string directionToString(Direction direction)
    {
        switch (direction)
        {
            case Direction::Up:
                return "Up";

            case Direction::Down:
                return "Down";

            case Direction::Left:
                return "Left";

            case Direction::Right:
                return "Right";

            case Direction::Stay:
            default:
                return "Stay";
        }
    }
}

/*
    Returns whether this action points to a real target tile.
*/
bool Action::hasTarget() const
{
    return targetX >= 0 && targetY >= 0;
}

/*
    Returns whether this action is a movement action.
*/
bool Action::isMovement() const
{
    return type == ActionType::Move;
}

/*
    Returns whether this action is a mating action.
*/
bool Action::isMate() const
{
    return type == ActionType::Mate;
}

/*
    Returns whether this action asks the entity to do nothing.
*/
bool Action::isStay() const
{
    return type == ActionType::Stay;
}

/*
    Converts the action into readable text for debugging.
    Additionally target based actions include their target coordinates.
*/
std::string Action::toString() const
{
    switch (type)
    {
        case ActionType::Stay:
            return "Stay";

        case ActionType::Move:
            return "Move(" + directionToString(direction) + ")";

        case ActionType::Eat:
            return "Eat(" + std::to_string(targetX) + ", " + std::to_string(targetY) + ")";

        case ActionType::Drink:
            return "Drink(" + std::to_string(targetX) + ", " + std::to_string(targetY) + ")";

        case ActionType::Attack:
            return "Attack(" + std::to_string(targetX) + ", " + std::to_string(targetY) + ")";

        case ActionType::PickUp:
            return "PickUp";

        case ActionType::Drop:
            return "Drop";

        case ActionType::Mate:
            return "Mate(" + std::to_string(targetX) + ", " + std::to_string(targetY) + ")";

        default:
            return "Unknown";
    }
}

/*
    Creates an action where the entity does nothing.
*/
Action Action::stay()
{
    return Action{};
}

/*
    Creates an action that moves the entity one tile in a direction.
*/
Action Action::move(Direction direction)
{
    Action action;
    action.type = ActionType::Move;
    action.direction = direction;
    return action;
}

/*
    Creates an action that eats from a target tile.
*/
Action Action::eat(int x, int y)
{
    Action action;
    action.type = ActionType::Eat;
    action.targetX = x;
    action.targetY = y;
    return action;
}

/*
    Creates an action that drinks from a target tile.
*/
Action Action::drink(int x, int y)
{
    Action action;
    action.type = ActionType::Drink;
    action.targetX = x;
    action.targetY = y;
    return action;
}

/*
    Creates an action that attacks a target tile.
*/
Action Action::attack(int x, int y)
{
    Action action;
    action.type = ActionType::Attack;
    action.targetX = x;
    action.targetY = y;
    return action;
}

/*
    Creates an action that picks up an item from the entity's current tile.
*/
Action Action::pickUp()
{
    Action action;
    action.type = ActionType::PickUp;
    return action;
}

/*
    Creates an action that drops the carried item onto the entity's current tile.
*/
Action Action::drop()
{
    Action action;
    action.type = ActionType::Drop;
    return action;
}

/*
    Creates an action that mates with an adjacent target entity.
*/
Action Action::mate(int x, int y)
{
    Action action;
    action.type = ActionType::Mate;
    action.targetX = x;
    action.targetY = y;
    return action;
}