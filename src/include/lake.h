#pragma once

#include <set>

#include "clumps.h"
#include "gameworld.h"

class Lake : public Clump
{
public:
    Lake(
        int centerX,
        int centerY,
        int numWaterBlocks,
        int gridSize,
        int cellSize
    );

    void addToWorld(GameWorld& world) const;
    void draw(GameWorld& world) const;

private:
    int cellSize;
    std::set<GridPos> sandCells;

    void generateSandBoundary();
};