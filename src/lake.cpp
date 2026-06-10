#include "lake.h"

Lake::Lake(
    int centerX,
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
    std::vector<GridPos> directions = {
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

void Lake::addToWorld(GameWorld& world) const
{
    for (const auto& sand : sandCells)
    {
        if (world.isInsideGrid(sand.x, sand.y))
        {
            world.setTile(sand.x, sand.y, TileType::Sand);
        }
    }

    for (const auto& water : cells)
    {
        if (world.isInsideGrid(water.x, water.y))
        {
            world.setTile(water.x, water.y, TileType::Water);
        }
    }
}

void Lake::draw(GameWorld& world) const
{
    for (const auto& sand : sandCells)
    {
        world.drawTile(sand.x, sand.y, TileType::Sand);
    }

    for (const auto& water : cells)
    {
        world.drawTile(water.x, water.y, TileType::Water);
    }
}