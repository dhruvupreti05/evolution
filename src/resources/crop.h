#pragma once

#include <vector>

#include "core/gameworld.h"

struct CropPosition
{
    int x;
    int y;
};

class Crop
{
public:
    static void update(GameWorld& world);
    static void drawCrops(GameWorld& world);

    static void addCropAt(int x, int y);
    static void removeCropAt(int x, int y);

    static void clearAll(GameWorld& world);

    static int getCount();

private:
    static std::vector<CropPosition> crops;

    static void trySpawnCrop(GameWorld& world);
};