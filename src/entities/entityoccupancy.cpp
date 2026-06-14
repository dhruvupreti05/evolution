#include "entities/entityoccupancy.h"

#include "core/gameworld.h"
#include "entities/human.h"
#include "entities/predator.h"

bool EntityOccupancy::built = false;
int EntityOccupancy::gridWidth = 1;
int EntityOccupancy::gridHeight = 1;

std::vector<Human*> EntityOccupancy::humansByPosition;
std::vector<Predator*> EntityOccupancy::predatorsByPosition;

/*
    Turns a grid position into a single vector index.
*/
int EntityOccupancy::makeKey(int x, int y)
{
    return y * gridWidth + x;
}

/*
    Checks whether a flattened position index is inside a lookup vector.
*/
bool EntityOccupancy::isValidKey(int key, int vectorSize)
{
    return key >= 0 && key < vectorSize;
}

/*
    Checks whether a grid position is inside the occupancy grid.
*/
bool EntityOccupancy::isInsideOccupancyGrid(int x, int y)
{
    return x >= 0 && x < gridWidth && y >= 0 && y < gridHeight;
}

/*
    Rebuilds the full position lookup from the current living humans and predators.
    This is useful after large world changes or after many entities have moved.
*/
void EntityOccupancy::rebuild(GameWorld& world)
{
    gridWidth = world.getGridWidth();
    gridHeight = world.getGridHeight();

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

/*
    Returns whether the occupancy lookup has been initialized.
*/
bool EntityOccupancy::hasBeenBuilt()
{
    return built;
}

/*
    Returns the living human at a grid position, or nullptr if there is none.
*/
Human* EntityOccupancy::getHumanAt(int x, int y)
{
    if (!built || !isInsideOccupancyGrid(x, y))
    {
        return nullptr;
    }

    int key = makeKey(x, y);

    if (!isValidKey(key, static_cast<int>(humansByPosition.size())))
    {
        return nullptr;
    }

    return humansByPosition[key];
}

/*
    Returns the living predator at a grid position, or nullptr if there is none.
*/
Predator* EntityOccupancy::getPredatorAt(int x, int y)
{
    if (!built || !isInsideOccupancyGrid(x, y))
    {
        return nullptr;
    }

    int key = makeKey(x, y);

    if (!isValidKey(key, static_cast<int>(predatorsByPosition.size())))
    {
        return nullptr;
    }

    return predatorsByPosition[key];
}

/*
    Checks whether a living human is on a tile.
*/
bool EntityOccupancy::hasHumanAt(int x, int y)
{
    return getHumanAt(x, y) != nullptr;
}

/*
    Checks whether a living predator is on a tile.
*/
bool EntityOccupancy::hasPredatorAt(int x, int y)
{
    return getPredatorAt(x, y) != nullptr;
}

/*
    Checks whether a tile is occupied by any living entity.
*/
bool EntityOccupancy::isBlockedAt(int x, int y)
{
    return hasHumanAt(x, y) || hasPredatorAt(x, y);
}

/*
    Updates one human's old and new positions without rebuilding the whole lookup.
*/
void EntityOccupancy::updateHumanPosition(Human& human, int oldX, int oldY)
{
    if (!built)
    {
        return;
    }

    if (isInsideOccupancyGrid(oldX, oldY))
    {
        int oldKey = makeKey(oldX, oldY);

        // Only clears the old tile if this exact human is still recorded there.
        if (isValidKey(oldKey, static_cast<int>(humansByPosition.size())) && humansByPosition[oldKey] == &human)
        {
            humansByPosition[oldKey] = nullptr;
        }
    }

    if (isInsideOccupancyGrid(human.getX(), human.getY()))
    {
        int newKey = makeKey(human.getX(), human.getY());

        if (isValidKey(newKey, static_cast<int>(humansByPosition.size())))
        {
            humansByPosition[newKey] = &human;
        }
    }
}

/*
    Updates one predator's old and new positions without rebuilding the whole lookup.
*/
void EntityOccupancy::updatePredatorPosition(Predator& predator, int oldX, int oldY)
{
    if (!built)
    {
        return;
    }

    if (isInsideOccupancyGrid(oldX, oldY))
    {
        int oldKey = makeKey(oldX, oldY);

        // Only clears the old tile if this exact predator is still recorded there.
        if (isValidKey(oldKey, static_cast<int>(predatorsByPosition.size())) && predatorsByPosition[oldKey] == &predator)
        {
            predatorsByPosition[oldKey] = nullptr;
        }
    }

    if (isInsideOccupancyGrid(predator.getX(), predator.getY()))
    {
        int newKey = makeKey(predator.getX(), predator.getY());

        if (isValidKey(newKey, static_cast<int>(predatorsByPosition.size())))
        {
            predatorsByPosition[newKey] = &predator;
        }
    }
}