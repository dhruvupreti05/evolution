#include "brain/brain.h"
#include "brain/predator-smart-brain.h"
#include "entities/predator.h"

#include "core/config.h"
#include "entities/entity.h"
#include "entities/human.h"
#include "entities/crop.h"

#include <cstdlib>
#include <cmath>



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
    Predator* predator = dynamic_cast<Predator*>(&entity);

    if (predator != nullptr && rand() % 20 == 0)
    {
        Predator* mate = Predator::getAdjacentLivingPredator(
            predator->getX(),
            predator->getY(),
            predator
        );

        if (mate != nullptr)
        {
            return Action::mate(mate->getX(), mate->getY());
        }
    }

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