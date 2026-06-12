#include "entities/brain.h"

#include "core/config.h"
#include "entities/entity.h"
#include "entities/human.h"
#include "entities/crop.h"

#include <cstdlib>
#include <cmath>

Direction RandomBrain::randomDirection() const
{
    int choice = rand() % 5;

    switch (choice)
    {
        case 1: return Direction::Up;
        case 2: return Direction::Down;
        case 3: return Direction::Left;
        case 4: return Direction::Right;
        case 0:
        default: return Direction::Stay;
    }
}

void RandomBrain::getNeighbor(
    int x,
    int y,
    Direction direction,
    int& outX,
    int& outY
) const
{
    outX = x;
    outY = y;

    switch (direction)
    {
        case Direction::Up:
            outY--;
            break;
        case Direction::Down:
            outY++;
            break;
        case Direction::Left:
            outX--;
            break;
        case Direction::Right:
            outX++;
            break;
        case Direction::Stay:
        default:
            break;
    }
}

Action RandomBrain::chooseAction(Entity& entity, GameWorld& world)
{
    Direction direction = randomDirection();

    if (direction == Direction::Stay)
    {
        return Action::stay();
    }

    int targetX;
    int targetY;
    getNeighbor(entity.getX(), entity.getY(), direction, targetX, targetY);

    if (!world.isInsideGrid(targetX, targetY))
    {
        return Action::stay();
    }

    TileType tile = world.getTile(targetX, targetY);

    if (tile == TileType::Crop && rand() % 2 == 0)
    {
        return Action::eat(targetX, targetY);
    }

    if (tile == TileType::Water && rand() % 2 == 0)
    {
        return Action::drink(targetX, targetY);
    }

    return Action::move(direction);
}

Action ManualBrain::chooseAction(Entity& entity, GameWorld& world)
{
    Action action = nextAction;
    nextAction = Action::stay();
    return action;
}

void ManualBrain::setNextAction(const Action& action)
{
    nextAction = action;
}

Direction PredatorSmartBrain::randomDirection() const
{
    int choice = rand() % 5;

    switch (choice)
    {
        case 1: return Direction::Up;
        case 2: return Direction::Down;
        case 3: return Direction::Left;
        case 4: return Direction::Right;
        case 0:
        default: return Direction::Stay;
    }
}

int PredatorSmartBrain::manhattanDistance(int x1, int y1, int x2, int y2) const
{
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

Direction PredatorSmartBrain::directionToward(
    Entity& entity,
    GameWorld& world,
    int targetX,
    int targetY
) const
{
    int x = entity.getX();
    int y = entity.getY();

    if (targetX < x) return Direction::Left;
    if (targetX > x) return Direction::Right;
    if (targetY < y) return Direction::Up;
    if (targetY > y) return Direction::Down;

    return Direction::Stay;
}

Action PredatorSmartBrain::chooseAction(Entity& entity, GameWorld& world)
{
    if (entity.getThirst() <= Config::PREDATOR_THIRST_MODE_THRESHOLD)
    {
        return chooseThirstAction(entity, world);
    }

    return chooseHungerAction(entity, world);
}

Action PredatorSmartBrain::chooseThirstAction(Entity& entity, GameWorld& world)
{
    const int directions[4][2] = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1}
    };

    for (const auto& direction : directions)
    {
        int waterX = entity.getX() + direction[0];
        int waterY = entity.getY() + direction[1];

        if (!world.isInsideGrid(waterX, waterY))
        {
            continue;
        }

        if (world.getTile(waterX, waterY) == TileType::Water)
        {
            return Action::drink(waterX, waterY);
        }
    }

    int bestX = -1;
    int bestY = -1;
    int bestDistance = 0;

    for (int y = 0; y < world.getGridHeight(); ++y)
    {
        for (int x = 0; x < world.getGridWidth(); ++x)
        {
            if (world.getTile(x, y) != TileType::Water)
            {
                continue;
            }

            int distance = manhattanDistance(entity.getX(), entity.getY(), x, y);

            if (bestX == -1 || distance < bestDistance)
            {
                bestX = x;
                bestY = y;
                bestDistance = distance;
            }
        }
    }

    if (bestX == -1)
    {
        return Action::move(randomDirection());
    }

    return Action::move(directionToward(entity, world, bestX, bestY));
}

Action PredatorSmartBrain::chooseHungerAction(Entity& entity, GameWorld& world)
{
    Human* body = Human::getAdjacentEdibleBody(entity.getX(), entity.getY());

    if (body != nullptr)
    {
        return Action::eat(body->getX(), body->getY());
    }

    Human* livingHuman = Human::getAdjacentLivingHuman(entity.getX(), entity.getY());

    if (livingHuman != nullptr)
    {
        return Action::attack(livingHuman->getX(), livingHuman->getY());
    }

    Human* target = Human::getNearestLivingHumanOrBody(entity.getX(), entity.getY());

    if (target == nullptr)
    {
        return Action::move(randomDirection());
    }

    return Action::move(directionToward(entity, world, target->getX(), target->getY()));
}
