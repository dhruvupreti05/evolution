#include "brain/brain.h"
#include "brain/random-brain.h"
#include "core/gridutils.h"

#include "core/config.h"
#include "entities/entity.h"
#include "entities/human.h"
#include "entities/crop.h"
#include "entities/predator.h"

#include <cstdlib>
#include <cmath>

/*
    Chooses a mostly random action for the entity.
    Humans and predators may randomly try to mate first, then the entity randomly moves, eats, drinks, or stays.
*/
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
        Predator* mate = Predator::getAdjacentLivingPredator(predator->getX(), predator->getY(), predator);

        if (mate != nullptr)
        {
            return Action::mate(mate->getX(), mate->getY());
        }
    }

    Direction direction = GridUtils::randomDirection();

    if (direction == Direction::Stay)
    {
        return Action::stay();
    }

    int targetX;
    int targetY;
    GridUtils::getNeighbor(entity.getX(), entity.getY(), direction, targetX, targetY);

    if (!world.isInsideGrid(targetX, targetY))
    {
        return Action::stay();
    }

    TileType tile = world.getTile(targetX, targetY);

    // Random brains sometimes eat crops instead of just walking onto them.
    if (tile == TileType::Crop && rand() % 2 == 0)
    {
        return Action::eat(targetX, targetY);
    }

    // Same idea as crops: drinking is possible, but not guaranteed.
    if (tile == TileType::Water && rand() % 2 == 0)
    {
        return Action::drink(targetX, targetY);
    }

    return Action::move(direction);
}