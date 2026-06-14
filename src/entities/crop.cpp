#include "entities/crop.h"
#include "core/gridutils.h"
#include "core/config.h"

#include <algorithm>
#include <cstdlib>
#include <map>

std::vector<Crop> Crop::crops;
std::map<GridPos, int> Crop::cropIndexByPosition;

/*
    Creates a crop at a grid position.
    The Food base class controls how many ticks it takes to eat and how much hunger each tick restores.
*/
Crop::Crop(int gridX, int gridY) : Food(Config::TICKS_PER_MEAL_CROP, Config::HUNGER_PER_TICK_MEAL_CROP), x(gridX), y(gridY)
{
}

/*
    Returns the crop's grid x position.
*/
int Crop::getX() const
{
    return x;
}

/*
    Returns the crop's grid y position.
*/
int Crop::getY() const
{
    return y;
}

/*
    Tries to grow new crops each simulation tick.
    Multiple attempts are made so crop growth is random but still steady.
*/
void Crop::update(GameWorld& world)
{
    for (int i = 0; i < Config::FOOD_SPAWN_ATTEMPTS_PER_TICK; ++i)
    {
        trySpawnCrop(world);
    }
}

/*
    Attempts to spawn one crop on a random sand tile.
    The crop only appears if the tile is valid and the random chance passes.
*/
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
    indexCropAt(x, y);
}

/*
    Draws every crop currently stored in the crop list.
*/
void Crop::drawCrops(GameWorld& world)
{
    for (const auto& crop : crops)
    {
        world.drawTile(crop.x, crop.y, TileType::Crop);
    }
}

/*
    Adds a crop directly without checking the world tile.
    This is useful when another system already decided the position is valid.
*/
void Crop::addCropAt(int x, int y)
{
    crops.emplace_back(x, y);
    indexCropAt(x, y);
}

/*
    Removes a crop from the crop list and clears its tile in the world.
*/
void Crop::removeCropAt(GameWorld& world, int x, int y)
{
    removeCropAt(x, y);

    if (world.isInsideGrid(x, y) && world.getTile(x, y) == TileType::Crop)
    {
        world.setTile(x, y, TileType::Empty);
    }
}

/*
    Lets an entity eat one tick of a crop.
    The crop only disappears after all of its eating ticks are used up.
*/
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

/*
    Finds a crop at a specific grid position.
    The map avoids searching through every crop each time.
*/
Crop* Crop::getCropAt(int x, int y)
{
    auto it = cropIndexByPosition.find({x, y});

    if (it == cropIndexByPosition.end())
    {
        return nullptr;
    }

    int index = it->second;

    // Protects against stale map entries if the crop list and index ever get out of sync.
    if (index < 0 || index >= static_cast<int>(crops.size()))
    {
        return nullptr;
    }

    return &crops[index];
}

/*
    Returns the number of crops currently in the world.
*/
int Crop::getCount()
{
    return static_cast<int>(crops.size());
}

/*
    Removes every crop from the world and clears the crop lookup map.
*/
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
    cropIndexByPosition.clear();
}

/*
    Removes a crop from the crop list without touching the world tile.
*/
void Crop::removeCropAt(int x, int y)
{
    crops.erase(std::remove_if(crops.begin(), crops.end(), [x, y](const Crop& crop) 
    { 
        return crop.getX() == x && crop.getY() == y; 
    }), crops.end());

    // Removing from a vector can shift indexes, so the whole lookup map is rebuilt.
    rebuildCropIndex();
}

/*
    Returns the shared crop list.
*/
const std::vector<Crop>& Crop::getCrops()
{
    return crops;
}

/*
    Finds the closest crop to a position using grid distance.
*/
bool Crop::getNearestCropCell(int x, int y, int& cropX, int& cropY)
{
    bool found = false;
    int bestDistance = 0;

    for (const Crop& crop : crops)
    {
        int distance = GridUtils::manhattanDistance(x, y, crop.getX(), crop.getY());

        if (!found || distance < bestDistance)
        {
            found = true;
            bestDistance = distance;
            cropX = crop.getX();
            cropY = crop.getY();
        }
    }

    return found;
}

/*
    Recreates the position-to-index map from the current crop list.
*/
void Crop::rebuildCropIndex()
{
    cropIndexByPosition.clear();

    for (int i = 0; i < static_cast<int>(crops.size()); ++i)
    {
        cropIndexByPosition[{crops[i].getX(), crops[i].getY()}] = i;
    }
}

/*
    Adds one crop position to the lookup map.
    This assumes the crop was just added to the end of the crop list.
*/
void Crop::indexCropAt(int x, int y)
{
    cropIndexByPosition[{x, y}] = static_cast<int>(crops.size()) - 1;
}

/*
    Removes one crop position from the lookup map.
*/
void Crop::unindexCropAt(int x, int y)
{
    cropIndexByPosition.erase({x, y});
}