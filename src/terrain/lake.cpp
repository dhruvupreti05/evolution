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
        if (!world.isInsideGrid(sand.x, sand.y))
        {
            continue;
        }

        TileType currentTile = world.getTile(sand.x, sand.y);

        if (currentTile == TileType::Empty || currentTile == TileType::Sand)
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

void Lake::regenerateSandBoundary()
{
    sandCells.clear();
    generateSandBoundary();
}

bool Lake::containsWaterCell(int x, int y) const
{
    return cells.count({x, y}) > 0;
}

bool Lake::isAdjacentToWaterCell(int x, int y) const
{
    for (const auto& water : cells)
    {
        int dx = std::abs(water.x - x);
        int dy = std::abs(water.y - y);

        if (dx <= 1 && dy <= 1)
        {
            return true;
        }
    }

    return false;
}

void Lake::addWaterCell(int x, int y)
{
    cells.insert({x, y});
    regenerateSandBoundary();
}

void Lake::removeWaterCell(int x, int y)
{
    cells.erase({x, y});
    regenerateSandBoundary();
}

void Lake::rebuildLakeTerrain(GameWorld& world)
{
    for (int y = 0; y < world.getGridHeight(); ++y)
    {
        for (int x = 0; x < world.getGridWidth(); ++x)
        {
            TileType tile = world.getTile(x, y);

            if (tile == TileType::Water || tile == TileType::Sand)
            {
                world.setTile(x, y, TileType::Empty);
            }
        }
    }

    for (const auto& lake : lakes)
    {
        lake.addToWorld(world);
    }
}

bool Lake::removeWaterAt(GameWorld& world, int x, int y)
{
    for (auto it = lakes.begin(); it != lakes.end(); ++it)
    {
        if (!it->containsWaterCell(x, y))
        {
            continue;
        }

        it->removeWaterCell(x, y);

        if (it->getCells().empty())
        {
            lakes.erase(it);
        }

        rebuildLakeTerrain(world);
        return true;
    }

    return false;
}

bool Lake::placeWaterAt(GameWorld& world, int x, int y)
{
    TileType tile = world.getTile(x, y);

    if (tile != TileType::Empty && tile != TileType::Sand)
    {
        return false;
    }

    for (auto& lake : lakes)
    {
        if (lake.isAdjacentToWaterCell(x, y))
        {
            lake.addWaterCell(x, y);
            rebuildLakeTerrain(world);
            return true;
        }
    }

    Lake newLake(
        x,
        y,
        1,
        world.getGridWidth(),
        world.getGridHeight()
    );

    lakes.push_back(newLake);
    rebuildLakeTerrain(world);

    return true;
}