#include "entities/food.h"

/*
    Creates food with a set number of eating ticks and hunger gained per tick.
*/
Food::Food(int mealTicks, int hungerPerTick) : mealTicksRemaining(mealTicks), hungerPerTick(hungerPerTick)
{
}

/*
    Returns whether this food still has anything left to eat.
*/
bool Food::isEdible() const
{
    return mealTicksRemaining > 0;
}

/*
    Uses up one eating tick if food is still available.
*/
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

/*
    Returns whether all eating ticks have been used.
*/
bool Food::isFullyEaten() const
{
    return mealTicksRemaining <= 0;
}

/*
    Returns how much hunger this food restores per eating tick.
*/
int Food::getHungerPerTick() const
{
    return hungerPerTick;
}

/*
    Returns how many eating ticks are left.
*/
int Food::getMealTicksRemaining() const
{
    return mealTicksRemaining;
}