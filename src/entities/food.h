#pragma once

/*
    Base class for food that takes time to eat.
    Each eating tick gives hunger back until the food has no meal ticks left.
*/
class Food
{
public:
    Food(int mealTicks, int hungerPerTick);
    virtual ~Food() = default;

    virtual bool isEdible() const;

    bool eatOneTick();
    bool isFullyEaten() const;

    int getHungerPerTick() const;
    int getMealTicksRemaining() const;

protected:
    // Counts how many more eating ticks this food can provide.
    int mealTicksRemaining = 0;

    // Hunger restored each time something eats one tick of this food.
    int hungerPerTick = 0;
};