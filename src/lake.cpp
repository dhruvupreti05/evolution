#include "lake.h"
#include "config.h"

#include <cstdlib>

std::vector<Lake> Lake::lakes;

Lake::Lake(int centerX, int centerY, int numWaterBlocks, int gridSize)
    : Clump(centerX, centerY, numWaterBlocks, gridSize)
{
    generateSandBoundary();
}

void Lake::init(GameWorld& world)
{
    int gridSize = world.getGridSize();
    int buffer = Config::LAKE_SPAWN_BOUNDARY_BUFFER;

    int usableMin = buffer;
    int usableMax = gridSize - buffer - 1;
    int usableSize = usableMax - usableMin + 1;

    if (usableSize <= 0)
    {
        return;
    }

    int numLakes = Config::NUM_LAKES;

    int columns = static_cast<int>(std::ceil(std::sqrt(numLakes)));
    int rows = static_cast<int>(std::ceil(static_cast<double>(numLakes) / columns));

    for (int i = 0; i < numLakes; ++i)
    {
        int row = i / columns;
        int col = i % columns;

        int regionXStart = (col * usableSize) / columns;
        int regionXEnd = ((col + 1) * usableSize) / columns - 1;

        int regionYStart = (row * usableSize) / rows;
        int regionYEnd = ((row + 1) * usableSize) / rows - 1;

        int regionWidth = regionXEnd - regionXStart + 1;
        int regionHeight = regionYEnd - regionYStart + 1;

        int gridX = usableMin + regionXStart + rand() % regionWidth;
        int gridY = usableMin + regionYStart + rand() % regionHeight;

        int numWaterBlocks =
            rand() % Config::LAKE_BLOCK_RANGE + Config::MIN_LAKE_BLOCKS;

        lakes.emplace_back(
            gridX,
            gridY,
            numWaterBlocks,
            world.getGridSize()
        );
    }

    for (const auto& lake : lakes)
    {
        lake.addToWorld(world);
    }
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

void Lake::drawLakes(GameWorld& world)
{
    for (const auto& lake : lakes)
    {
        lake.draw(world);
    }
}

const std::set<GridPos>& Lake::getSandCells() const
{
    return sandCells;
}