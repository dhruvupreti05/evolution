#include "entities/food.h"

Food::Food(int mealTicks, int hungerPerTick)
    : mealTicksRemaining(mealTicks),
      hungerPerTick(hungerPerTick)
{
}

bool Food::isEdible() const
{
    return mealTicksRemaining > 0;
}

bool Food::eatOneTick()
{
    if (!isEdible())
    {
        return false;
    }

    mealTicksRemaining--;

    if (mealTicksRemaining < 0)
    {
        mealTicksRemaining = 0;
    }

    return true;
}

bool Food::isFullyEaten() const
{
    return mealTicksRemaining <= 0;
}

int Food::getHungerPerTick() const
{
    return hungerPerTick;
}

int Food::getMealTicksRemaining() const
{
    return mealTicksRemaining;
}