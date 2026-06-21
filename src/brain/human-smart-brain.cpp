#include "brain/human-smart-brain.h"

#include "core/config.h"
#include "core/gridutils.h"
#include "entities/crop.h"
#include "entities/entity.h"
#include "entities/entityoccupancy.h"
#include "entities/human.h"
#include "entities/predator.h"
#include "environment/lake.h"

#include <cstdlib>
#include <limits>

/*
    The decision function for the smart human.
    Priority:
    1. Critical thirst.
    2. Flee nearby predators.
    3. Sometimes mate, matching predator smart-brain style.
    4. Normal thirst.
    5. Eat or search for crops.
*/
Action HumanSmartBrain::chooseAction(Entity& entity, GameWorld& world)
{
    Human* human = dynamic_cast<Human*>(&entity);

    if (entity.getThirst() <= Config::HUMAN_CRITICAL_THIRST_MODE_THRESHOLD)
    {
        return chooseThirstAction(entity, world);
    }

    const Predator* nearestPredator = getNearestPredatorWithinRange(entity, Config::HUMAN_PREDATOR_DETECTION_RANGE);

    if (nearestPredator != nullptr)
    {
        return chooseFleeAction(entity, world, nearestPredator);
    }

    if (human != nullptr && human->canMateNow() && rand() % 20 == 0)
    {
        Human* mate = Human::getAdjacentMateCandidate(*human);

        if (mate != nullptr)
        {
            return Action::mate(mate->getX(), mate->getY());
        }
    }

    if (entity.getThirst() <= Config::HUMAN_THIRST_MODE_THRESHOLD)
    {
        return chooseThirstAction(entity, world);
    }

    return chooseFoodAction(entity, world);
}

/*
    Chooses what the human should do when it needs water.
    It drinks immediately if water is adjacent, otherwise it moves toward nearby water.
*/
Action HumanSmartBrain::chooseThirstAction(Entity& entity, GameWorld& world)
{
    for (const auto& direction : GridUtils::FOUR_DIRECTIONS)
    {
        int waterX = entity.getX() + direction.dx;
        int waterY = entity.getY() + direction.dy;

        if (!world.isInsideGrid(waterX, waterY))
        {
            continue;
        }

        if (world.getTile(waterX, waterY) == TileType::Water)
        {
            return Action::drink(waterX, waterY);
        }
    }

    int waterX;
    int waterY;

    bool foundWater = Lake::getNearestWaterCellWithinRange(world, entity.getX(), entity.getY(), Config::HUMAN_WATER_SEARCH_RANGE, waterX, waterY);

    if (!foundWater)
    {
        return Action::move(GridUtils::randomDirection());
    }

    return Action::move(GridUtils::directionToward(entity.getX(), entity.getY(), waterX, waterY));
}

/*
    Chooses what the human should do when it is ready to eat.
    Humans eat adjacent crops directly from the ground.
    If no adjacent crop exists, they move toward the nearest visible/available crop in range.
*/
Action HumanSmartBrain::chooseFoodAction(Entity& entity, GameWorld& world)
{
    Human* human = dynamic_cast<Human*>(&entity);

    if (human != nullptr && human->hasCropInInventory() && entity.getHunger() < Config::HUMAN_MAX_HUNGER)
    {
        return Action::eat(entity.getX(), entity.getY());
    }

    for (const auto& direction : GridUtils::FOUR_DIRECTIONS)
    {
        int cropX = entity.getX() + direction.dx;
        int cropY = entity.getY() + direction.dy;

        if (!world.isInsideGrid(cropX, cropY))
        {
            continue;
        }

        if (world.getTile(cropX, cropY) == TileType::Crop)
        {
            return Action::eat(cropX, cropY);
        }
    }

    const std::vector<Crop>& crops = Crop::getCrops();

    bool foundCrop = false;
    int bestCropX = 0;
    int bestCropY = 0;
    int bestDistance = Config::HUMAN_FOOD_SEARCH_RANGE + 1;

    for (const Crop& crop : crops)
    {
        int cropX = crop.getX();
        int cropY = crop.getY();

        if (!world.isInsideGrid(cropX, cropY))
        {
            continue;
        }

        // This prevents humans from trying to eat crops hidden under bodies or overwritten terrain.
        if (world.getTile(cropX, cropY) != TileType::Crop)
        {
            continue;
        }

        int distance = GridUtils::manhattanDistance(entity.getX(), entity.getY(), cropX, cropY);

        if (distance <= Config::HUMAN_FOOD_SEARCH_RANGE && distance < bestDistance)
        {
            foundCrop = true;
            bestDistance = distance;
            bestCropX = cropX;
            bestCropY = cropY;
        }
    }

    if (!foundCrop)
    {
        return Action::move(GridUtils::randomDirection());
    }

    return Action::move(GridUtils::directionToward(entity.getX(), entity.getY(), bestCropX, bestCropY));
}

/*
    Moves to the neighboring tile that maximizes distance from the nearest predator.
*/
Action HumanSmartBrain::chooseFleeAction(Entity& entity, GameWorld& world, const Predator* predator)
{
    if (predator == nullptr)
    {
        return Action::move(GridUtils::randomDirection());
    }

    Direction bestDirection = Direction::Stay;
    int bestDistance = GridUtils::manhattanDistance(entity.getX(), entity.getY(), predator->getX(), predator->getY());

    for (const auto& direction : GridUtils::FOUR_DIRECTIONS)
    {
        int nextX = entity.getX() + direction.dx;
        int nextY = entity.getY() + direction.dy;

        if (!world.isInsideGrid(nextX, nextY))
        {
            continue;
        }

        TileType tile = world.getTile(nextX, nextY);

        if (tile == TileType::Water || tile == TileType::Crop || tile == TileType::Body)
        {
            continue;
        }

        if (EntityOccupancy::isBlockedAt(nextX, nextY))
        {
            continue;
        }

        int distance = GridUtils::manhattanDistance(nextX, nextY, predator->getX(), predator->getY());

        if (distance > bestDistance)
        {
            bestDistance = distance;

            if (direction.dx == 1)
            {
                bestDirection = Direction::Right;
            }
            else if (direction.dx == -1)
            {
                bestDirection = Direction::Left;
            }
            else if (direction.dy == 1)
            {
                bestDirection = Direction::Down;
            }
            else if (direction.dy == -1)
            {
                bestDirection = Direction::Up;
            }
        }
    }

    if (bestDirection == Direction::Stay)
    {
        return Action::stay();
    }

    return Action::move(bestDirection);
}

/*
    Finds the nearest living predator within a limited detection range.
*/
const Predator* HumanSmartBrain::getNearestPredatorWithinRange(Entity& entity, int range)
{
    const Predator* bestPredator = nullptr;
    int bestDistance = range + 1;

    const std::vector<Predator>& predators = Predator::getPredators();

    for (const Predator& predator : predators)
    {
        if (predator.isDead())
        {
            continue;
        }

        int distance = GridUtils::manhattanDistance(entity.getX(), entity.getY(), predator.getX(), predator.getY());

        if (distance <= range && distance < bestDistance)
        {
            bestPredator = &predator;
            bestDistance = distance;
        }
    }

    return bestPredator;
}