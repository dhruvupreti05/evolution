#pragma once

#include <vector>

#include "gameworld.h"

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

private:
    static std::vector<FoodPosition> foods;

    static void trySpawnFood(GameWorld& world);
};