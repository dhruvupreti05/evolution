#pragma once

#include <vector>
#include <map>

#include "core/gameworld.h"
#include "entities/food.h"
#include "environment/clumps.h"

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

    static std::vector<Crop> crops;

    static void trySpawnCrop(GameWorld& world);

    static std::map<GridPos, int> cropIndexByPosition;

    static void rebuildCropIndex();
    static void indexCropAt(int x, int y);
    static void unindexCropAt(int x, int y);
};