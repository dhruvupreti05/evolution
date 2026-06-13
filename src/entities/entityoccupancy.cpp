#include "entities/entityoccupancy.h"

#include "core/gameworld.h"
#include "entities/human.h"
#include "entities/predator.h"

bool EntityOccupancy::built = false;
int EntityOccupancy::gridWidth = 1;

std::vector<Human*> EntityOccupancy::humansByPosition;
std::vector<Predator*> EntityOccupancy::predatorsByPosition;

int EntityOccupancy::makeKey(int x, int y)
{
    return y * gridWidth + x;
}

bool EntityOccupancy::isValidKey(int key, int vectorSize)
{
    return key >= 0 && key < vectorSize;
}

void EntityOccupancy::rebuild(GameWorld& world)
{
    gridWidth = world.getGridWidth();

    int totalCells = world.getGridWidth() * world.getGridHeight();

    humansByPosition.assign(totalCells, nullptr);
    predatorsByPosition.assign(totalCells, nullptr);

    std::vector<Human>& humans = Human::getHumans();

    for (Human& human : humans)
    {
        if (human.isDead())
        {
            continue;
        }

        if (!world.isInsideGrid(human.getX(), human.getY()))
        {
            continue;
        }

        int key = makeKey(human.getX(), human.getY());
        humansByPosition[key] = &human;
    }

    std::vector<Predator>& predators = Predator::getPredatorsMutable();

    for (Predator& predator : predators)
    {
        if (predator.isDead())
        {
            continue;
        }

        if (!world.isInsideGrid(predator.getX(), predator.getY()))
        {
            continue;
        }

        int key = makeKey(predator.getX(), predator.getY());
        predatorsByPosition[key] = &predator;
    }

    built = true;
}

bool EntityOccupancy::hasBeenBuilt()
{
    return built;
}

Human* EntityOccupancy::getHumanAt(int x, int y)
{
    if (!built)
    {
        return nullptr;
    }

    int key = makeKey(x, y);

    if (key < 0 || key >= static_cast<int>(humansByPosition.size()))
    {
        return nullptr;
    }

    return humansByPosition[key];
}

Predator* EntityOccupancy::getPredatorAt(int x, int y)
{
    if (!built)
    {
        return nullptr;
    }

    int key = makeKey(x, y);

    if (key < 0 || key >= static_cast<int>(predatorsByPosition.size()))
    {
        return nullptr;
    }

    return predatorsByPosition[key];
}

bool EntityOccupancy::hasHumanAt(int x, int y)
{
    return getHumanAt(x, y) != nullptr;
}

bool EntityOccupancy::hasPredatorAt(int x, int y)
{
    return getPredatorAt(x, y) != nullptr;
}

bool EntityOccupancy::isBlockedAt(int x, int y)
{
    return hasHumanAt(x, y) || hasPredatorAt(x, y);
}

void EntityOccupancy::updateHumanPosition(Human& human, int oldX, int oldY)
{
    if (!built)
    {
        return;
    }

    int oldKey = makeKey(oldX, oldY);

    if (
        isValidKey(oldKey, static_cast<int>(humansByPosition.size())) &&
        humansByPosition[oldKey] == &human
    )
    {
        humansByPosition[oldKey] = nullptr;
    }

    int newKey = makeKey(human.getX(), human.getY());

    if (isValidKey(newKey, static_cast<int>(humansByPosition.size())))
    {
        humansByPosition[newKey] = &human;
    }
}

void EntityOccupancy::updatePredatorPosition(Predator& predator, int oldX, int oldY)
{
    if (!built)
    {
        return;
    }

    int oldKey = makeKey(oldX, oldY);

    if (
        isValidKey(oldKey, static_cast<int>(predatorsByPosition.size())) &&
        predatorsByPosition[oldKey] == &predator
    )
    {
        predatorsByPosition[oldKey] = nullptr;
    }

    int newKey = makeKey(predator.getX(), predator.getY());

    if (isValidKey(newKey, static_cast<int>(predatorsByPosition.size())))
    {
        predatorsByPosition[newKey] = &predator;
    }
}