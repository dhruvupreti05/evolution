#pragma once

#include <set>
#include <vector>
#include <map>

#include "environment/clumps.h"
#include "core/gameworld.h"

/*
    Lake terrain feature built from a clump of water cells.
    Lakes also manage their surrounding sand, water pickup/drop, drinking progress, floods, and droughts.
*/
class Lake : public Clump
{
public:
    Lake(int centerX, int centerY, int numWaterBlocks, int gridWidth, int gridHeight);

    static void init(GameWorld& world);
    static void drawLakes(GameWorld& world);

    void addToWorld(GameWorld& world) const;
    void draw(GameWorld& world) const;

    static bool getNearestWaterCell(int x, int y, int& waterX, int& waterY);
    static bool getNearestWaterCellWithinRange(const GameWorld& world, int x, int y, int range, int& waterX, int& waterY);

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

    // Tracks how many drinking ticks are left for each water tile being used.
    static std::map<GridPos, int> waterDrinkTicksRemaining;

    // Sand cells are generated around the lake's water cells.
    std::set<GridPos> sandCells;

    void generateSandBoundary();
    void regenerateSandBoundary();

    bool containsWaterCell(int x, int y) const;
    bool isAdjacentToWaterCell(int x, int y) const;

    void addWaterCell(int x, int y);
    void removeWaterCell(int x, int y);

    // Rewrites the world tiles to match the current lake and sand cells.
    static void rebuildLakeTerrain(GameWorld& world);

    void dryOneLayer();
    void floodOneLayer();
};