#pragma once

#include <SFML/Graphics.hpp>
#include <set>

#include "clumps.h"
#include "block.h"

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

    void draw(sf::RenderWindow& window) const;

private:
    int cellSize;
    std::set<GridPos> sandCells;

    void generateSandBoundary();
};