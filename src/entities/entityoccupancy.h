#pragma once

#include <vector>

#include "core/gameworld.h"

class Human;
class Predator;

/*
    Tracks which entities are standing on each grid tile.
    This makes position lookups fast without searching every human or predator each time.
*/
class EntityOccupancy
{
public:
    static void rebuild(GameWorld& world);

    static bool hasBeenBuilt();

    static Human* getHumanAt(int x, int y);
    static Predator* getPredatorAt(int x, int y);

    static bool hasHumanAt(int x, int y);
    static bool hasPredatorAt(int x, int y);
    static bool isBlockedAt(int x, int y);

    static void updateHumanPosition(Human& human, int oldX, int oldY);
    static void updatePredatorPosition(Predator& predator, int oldX, int oldY);

private:
    static bool built;
    static int gridWidth;

    static int gridHeight;
    static bool isInsideOccupancyGrid(int x, int y);

    // Turns a 2D grid position into one index for the position vectors.
    static int makeKey(int x, int y);

    // Protects against using a position key outside the vector bounds.
    static bool isValidKey(int key, int vectorSize);

    // Stores entity pointers by tile position for quick lookup.
    static std::vector<Human*> humansByPosition;
    static std::vector<Predator*> predatorsByPosition;
};