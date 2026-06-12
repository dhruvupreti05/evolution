#include "brain/brain.h"
#include "brain/random-brain.h"

#include "core/config.h"
#include "entities/entity.h"
#include "entities/human.h"
#include "entities/crop.h"
#include "entities/predator.h"

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
    Human* human = dynamic_cast<Human*>(&entity);

    if (human != nullptr && rand() % 20 == 0)
    {
        Human* mate = Human::getAdjacentMateCandidate(*human);

        if (mate != nullptr)
        {
            return Action::mate(mate->getX(), mate->getY());
        }
    }

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