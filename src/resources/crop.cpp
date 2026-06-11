#include "crop.h"
#include "core/config.h"

#include <algorithm>
#include <cstdlib>

std::vector<CropPosition> Crop::crops;

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

    crops.push_back({x, y});
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
    crops.push_back({x, y});
}

void Crop::removeCropAt(int x, int y)
{
    crops.erase(
        std::remove_if(
            crops.begin(),
            crops.end(),
            [x, y](const CropPosition& crop)
            {
                return crop.x == x && crop.y == y;
            }
        ),
        crops.end()
    );
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