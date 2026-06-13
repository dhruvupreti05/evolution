#include "entities/entityoccupancy.h"

#include "entities/human.h"
#include "entities/predator.h"

#include <algorithm>

bool EntityOccupancy::built = false;
int EntityOccupancy::maxKey = 0;

std::vector<Human*> EntityOccupancy::humansByPosition;
std::vector<Predator*> EntityOccupancy::predatorsByPosition;

int EntityOccupancy::makeKey(int x, int y)
{
    return y * 10000 + x;
}

void EntityOccupancy::ensureKeyExists(int key)
{
    if (key < 0)
    {
        return;
    }

    if (key >= static_cast<int>(humansByPosition.size()))
    {
        humansByPosition.resize(key + 1, nullptr);
    }

    if (key >= static_cast<int>(predatorsByPosition.size()))
    {
        predatorsByPosition.resize(key + 1, nullptr);
    }
}

void EntityOccupancy::rebuild()
{
    std::fill(humansByPosition.begin(), humansByPosition.end(), nullptr);
    std::fill(predatorsByPosition.begin(), predatorsByPosition.end(), nullptr);

    std::vector<Human>& humans = Human::getHumans();

    for (Human& human : humans)
    {
        if (human.isDead())
        {
            continue;
        }

        int key = makeKey(human.getX(), human.getY());
        ensureKeyExists(key);
        humansByPosition[key] = &human;
    }

    const std::vector<Predator>& constPredators = Predator::getPredators();

    for (const Predator& predatorConst : constPredators)
    {
        if (predatorConst.isDead())
        {
            continue;
        }

        Predator* predator = const_cast<Predator*>(&predatorConst);

        int key = makeKey(predator->getX(), predator->getY());
        ensureKeyExists(key);
        predatorsByPosition[key] = predator;
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