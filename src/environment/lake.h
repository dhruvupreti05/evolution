#pragma once

#include <set>
#include <vector>
#include <map>

#include "environment/clumps.h"
#include "core/gameworld.h"

class Lake : public Clump
{
public:
    Lake(
        int centerX,
        int centerY,
        int numWaterBlocks,
        int gridWidth,
        int gridHeight
    );

    static void init(GameWorld& world);
    static void drawLakes(GameWorld& world);

    void addToWorld(GameWorld& world) const;
    void draw(GameWorld& world) const;

    const std::set<GridPos>& getSandCells() const;

    static bool removeWaterAt(GameWorld& world, int x, int y);
    static bool placeWaterAt(GameWorld& world, int x, int y);

    static void floodAll(GameWorld& world, int layers);
    static void dryAll(GameWorld& world, int layers);

    static int getTotalWaterBlocks();

    static bool drinkWaterAt(GameWorld& world, int x, int y);
    static void resetWaterDrinkingProgressForMissingWater();

private:
    static std::vector<Lake> lakes;
    static std::map<GridPos, int> waterDrinkTicksRemaining;

    std::set<GridPos> sandCells;

    void generateSandBoundary();
    void regenerateSandBoundary();

    bool containsWaterCell(int x, int y) const;
    bool isAdjacentToWaterCell(int x, int y) const;

    void addWaterCell(int x, int y);
    void removeWaterCell(int x, int y);

    static void rebuildLakeTerrain(GameWorld& world);

    void dryOneLayer();
    void floodOneLayer();
};