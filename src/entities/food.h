#pragma once

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
    int mealTicksRemaining = 0;
    int hungerPerTick = 0;
};