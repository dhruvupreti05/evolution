#pragma once

#include <set>
#include <vector>
#include <cmath>

#include "clumps.h"
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

private:
    static std::vector<Lake> lakes;

    std::set<GridPos> sandCells;

    void generateSandBoundary();
};