#pragma once

#include <vector>

#include "core/gameworld.h"

struct FoodPosition
{
    int x;
    int y;
};

class Food
{
public:
    static void update(GameWorld& world);
    static void drawFoods(GameWorld& world);

    static void addFoodAt(int x, int y);
    static void removeFoodAt(int x, int y);

    static void clearAll(GameWorld& world);

    static int getCount();

private:
    static std::vector<FoodPosition> foods;

    static void trySpawnFood(GameWorld& world);
};