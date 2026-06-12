#include "entities/action.h"

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
