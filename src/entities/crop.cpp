#include "entities/crop.h"

#include "core/config.h"

#include <algorithm>
#include <cstdlib>

std::vector<Crop> Crop::crops;

Crop::Crop(int gridX, int gridY)
    : Food(Config::TICKS_PER_MEAL_CROP, Config::HUNGER_PER_TICK_MEAL_CROP),
      x(gridX),
      y(gridY)
{
}

int Crop::getX() const
{
    return x;
}

int Crop::getY() const
{
    return y;
}

void Crop::update(GameWorld& world)
{
    for (int i = 0; i < Config::FOOD_SPAWN_ATTEMPTS_PER_TICK; ++i)
    {
        trySpawnCrop(world);
    }
}

void Crop::trySpawnCrop(GameWorld& world)
{
    int x = rand() % world.getGridWidth();
    int y = rand() % world.getGridHeight();

    if (world.getTile(x, y) != TileType::Sand)
    {
        return;
    }

    int chance = rand() % 100;

    if (chance >= Config::FOOD_SPAWN_CHANCE_PERCENT)
    {
        return;
    }

    world.setTile(x, y, TileType::Crop);
    crops.emplace_back(x, y);
}

void Crop::drawCrops(GameWorld& world)
{
    for (const auto& crop : crops)
    {
        world.drawTile(crop.x, crop.y, TileType::Crop);
    }
}

void Crop::addCropAt(int x, int y)
{
    crops.emplace_back(x, y);
}

void Crop::removeCropAt(GameWorld& world, int x, int y)
{
    removeCropAt(x, y);

    if (world.isInsideGrid(x, y) && world.getTile(x, y) == TileType::Crop)
    {
        world.setTile(x, y, TileType::Empty);
    }
}


bool Crop::eatCropAt(GameWorld& world, int x, int y, int& hungerGain)
{
    Crop* crop = getCropAt(x, y);

    if (crop == nullptr)
    {
        return false;
    }

    if (!crop->eatOneTick())
    {
        return false;
    }

    hungerGain = crop->getHungerPerTick();

    if (crop->isFullyEaten())
    {
        removeCropAt(world, x, y);
    }

    return true;
}

Crop* Crop::getCropAt(int x, int y)
{
    for (auto& crop : crops)
    {
        if (crop.x == x && crop.y == y)
        {
            return &crop;
        }
    }

    return nullptr;
}

int Crop::getCount()
{
    return static_cast<int>(crops.size());
}

void Crop::clearAll(GameWorld& world)
{
    for (const auto& crop : crops)
    {
        if (!world.isInsideGrid(crop.x, crop.y))
        {
            continue;
        }

        if (world.getTile(crop.x, crop.y) == TileType::Crop)
        {
            world.setTile(crop.x, crop.y, TileType::Empty);
        }
    }

    crops.clear();
}

void Crop::removeCropAt(int x, int y)
{
    crops.erase(
        std::remove_if(
            crops.begin(),
            crops.end(),
            [x, y](const Crop& crop)
            {
                return crop.getX() == x && crop.getY() == y;
            }
        ),
        crops.end()
    );
}
