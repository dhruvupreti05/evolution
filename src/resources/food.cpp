#include "food.h"
#include "core/config.h"

#include <cstdlib>

std::vector<FoodPosition> Food::foods;

void Food::update(GameWorld& world)
{
    for (int i = 0; i < Config::FOOD_SPAWN_ATTEMPTS_PER_TICK; ++i)
    {
        trySpawnFood(world);
    }
}

void Food::trySpawnFood(GameWorld& world)
{
    int x = rand() % world.getGridSize();
    int y = rand() % world.getGridSize();

    if (world.getTile(x, y) != TileType::Sand)
    {
        return;
    }

    int chance = rand() % 100;

    if (chance >= Config::FOOD_SPAWN_CHANCE_PERCENT)
    {
        return;
    }

    world.setTile(x, y, TileType::Food);

    foods.push_back({x, y});
}

void Food::drawFoods(GameWorld& world)
{
    for (const auto& food : foods)
    {
        world.drawTile(food.x, food.y, TileType::Food);
    }
}