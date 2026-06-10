#include "lake.h"

Lake::Lake(int centerX,
    int centerY,
    int numWaterBlocks,
    int gridSize,
    int cellSize
)
    : Clump(centerX, centerY, numWaterBlocks, gridSize),
      cellSize(cellSize)
{
    generateSandBoundary();
}

void Lake::generateSandBoundary()
{
    std::vector<GridPos> directions = 
    {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1},
        {1, 1},
        {1, -1},
        {-1, 1},
        {-1, -1}
    };

    for (const auto& waterCell : cells)
    {
        for (const auto& dir : directions)
        {
            GridPos neighbor = {
                waterCell.x + dir.x,
                waterCell.y + dir.y
            };

            if (cells.count(neighbor) == 0)
            {
                sandCells.insert(neighbor);
            }
        }
    }
}

void Lake::draw(sf::RenderWindow& window) const
{
    for (const auto& sand : sandCells)
    {
        Block block(sand.x, sand.y, cellSize, sf::Color(194, 178, 128));

        block.draw(window);
    }

    for (const auto& water : cells)
    {
        Block block(water.x, water.y, cellSize, sf::Color::Blue);

        block.draw(window);
    }
}