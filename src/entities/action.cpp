#include "entities/action.h"

#include <string>

namespace
{
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

bool Action::hasTarget() const
{
    return targetX >= 0 && targetY >= 0;
}

bool Action::isMovement() const
{
    return type == ActionType::Move;
}

bool Action::isMate() const
{
    return type == ActionType::Mate;
}

bool Action::isStay() const
{
    return type == ActionType::Stay;
}

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

Action Action::stay()
{
    return Action{};
}

Action Action::move(Direction direction)
{
    Action action;
    action.type = ActionType::Move;
    action.direction = direction;
    return action;
}

Action Action::eat(int x, int y)
{
    Action action;
    action.type = ActionType::Eat;
    action.targetX = x;
    action.targetY = y;
    return action;
}

Action Action::drink(int x, int y)
{
    Action action;
    action.type = ActionType::Drink;
    action.targetX = x;
    action.targetY = y;
    return action;
}

Action Action::attack(int x, int y)
{
    Action action;
    action.type = ActionType::Attack;
    action.targetX = x;
    action.targetY = y;
    return action;
}

Action Action::pickUp()
{
    Action action;
    action.type = ActionType::PickUp;
    return action;
}

Action Action::drop()
{
    Action action;
    action.type = ActionType::Drop;
    return action;
}

Action Action::mate(int x, int y)
{
    Action action;
    action.type = ActionType::Mate;
    action.targetX = x;
    action.targetY = y;
    return action;
}