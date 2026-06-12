#pragma once

#include <vector>

#include "core/gameworld.h"
#include "entities/food.h"

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

private:
    int x;
    int y;

    static std::vector<Crop> crops;

    static void trySpawnCrop(GameWorld& world);
};