#include "lake.h"
#include "core/config.h"

#include <cstdlib>
#include <cmath>
#include <limits>

std::vector<Lake> Lake::lakes;

struct LakeSpawnPoint
{
    int x;
    int y;
};

Lake::Lake(
    int centerX,
    int centerY,
    int numWaterBlocks,
    int gridWidth,
    int gridHeight
)
    : Clump(centerX, centerY, numWaterBlocks, gridWidth, gridHeight)
{
    generateSandBoundary();
}

void Lake::init(GameWorld& world)
{
    int gridWidth = world.getGridWidth();
    int gridHeight = world.getGridHeight();

    int buffer = Config::LAKE_SPAWN_BOUNDARY_BUFFER;

    int usableMinX = buffer;
    int usableMaxX = gridWidth - buffer - 1;

    int usableMinY = buffer;
    int usableMaxY = gridHeight - buffer - 1;

    if (usableMaxX < usableMinX || usableMaxY < usableMinY)
    {
        return;
    }

    int numLakes = Config::NUM_LAKES;

    std::vector<LakeSpawnPoint> candidates;

    for (int y = usableMinY; y <= usableMaxY; ++y)
    {
        for (int x = usableMinX; x <= usableMaxX; ++x)
        {
            candidates.push_back({x, y});
        }
    }

    if (candidates.empty())
    {
        return;
    }

    std::vector<LakeSpawnPoint> chosenCenters;

    int firstIndex = rand() % candidates.size();
    chosenCenters.push_back(candidates[firstIndex]);
    candidates.erase(candidates.begin() + firstIndex);

    while (
        static_cast<int>(chosenCenters.size()) < numLakes &&
        !candidates.empty()
    )
    {
        double bestDistanceToNearestLake = -1.0;
        int bestCandidateIndex = -1;

        for (int i = 0; i < static_cast<int>(candidates.size()); ++i)
        {
            LakeSpawnPoint candidate = candidates[i];

            double distanceToNearestLake = std::numeric_limits<double>::max();

            for (const auto& center : chosenCenters)
            {
                int dx = candidate.x - center.x;
                int dy = candidate.y - center.y;

                double distanceSquared = dx * dx + dy * dy;

                if (distanceSquared < distanceToNearestLake)
                {
                    distanceToNearestLake = distanceSquared;
                }
            }

            if (distanceToNearestLake > bestDistanceToNearestLake)
            {
                bestDistanceToNearestLake = distanceToNearestLake;
                bestCandidateIndex = i;
            }
        }

        if (bestCandidateIndex == -1)
        {
            break;
        }

        chosenCenters.push_back(candidates[bestCandidateIndex]);
        candidates.erase(candidates.begin() + bestCandidateIndex);
    }

    for (const auto& center : chosenCenters)
    {
        int numWaterBlocks =
            rand() % Config::LAKE_BLOCK_RANGE + Config::MIN_LAKE_BLOCKS;

        lakes.emplace_back(
            center.x,
            center.y,
            numWaterBlocks,
            world.getGridWidth(),
            world.getGridHeight()
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