#pragma once

#include <vector>
#include <map>

#include "core/gameworld.h"
#include "entities/food.h"
#include "environment/clumps.h"

/*
    Food object that grows on the world grid.
    Crops are stored in one shared list and looked up by position when entities eat or search for food.
*/
class Crop : public Food
{
public:
    Crop(int gridX, int gridY);

    int getX() const;
    int getY() const;

    static void update(GameWorld& world);
    static void drawCrops(GameWorld& world);

    static void addCropAt(int x, int y);
    static void removeCropAt(GameWorld& world, int x, int y);
    static void removeCropAt(int x, int y);

    static bool eatCropAt(GameWorld& world, int x, int y, int& hungerGain);
    static Crop* getCropAt(int x, int y);

    static void clearAll(GameWorld& world);

    static int getCount();

    static const std::vector<Crop>& getCrops();
    static bool getNearestCropCell(int x, int y, int& cropX, int& cropY);

private:
    int x;
    int y;

    // Shared list of every crop currently alive in the world.
    static std::vector<Crop> crops;

    static void trySpawnCrop(GameWorld& world);

    // Fast lookup from grid position to the crop's index in the crops vector.
    static std::map<GridPos, int> cropIndexByPosition;

    // Rebuilds the position lookup after crop indexes may have shifted.
    static void rebuildCropIndex();

    static void indexCropAt(int x, int y);
    static void unindexCropAt(int x, int y);
};