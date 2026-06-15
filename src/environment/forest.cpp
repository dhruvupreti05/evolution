#include "environment/forest.h"
#include "core/config.h"

#include <cstdlib>
#include <limits>

std::vector<Forest> Forest::forests;

/*
    Candidate center point for placing a forest.
*/
struct ForestSpawnPoint
{
    int x;
    int y;
};

/*
    Creates a forest clump and removes any cells that would overlap existing terrain.
*/
Forest::Forest(int centerX, int centerY, int numForestBlocks, const GameWorld& world) : Clump(centerX, centerY, numForestBlocks, world.getGridWidth(), world.getGridHeight())
{
    keepOnlyEmptyWorldCells(world);
}

/*
    Creates the starting forests after lakes have already been placed.
    Centers are spread out so forests do not all cluster in one area.
*/
void Forest::init(GameWorld& world)
{
    int gridWidth = world.getGridWidth();
    int gridHeight = world.getGridHeight();

    int buffer = Config::FOREST_SPAWN_BOUNDARY_BUFFER;

    int usableMinX = buffer;
    int usableMaxX = gridWidth - buffer - 1;

    int usableMinY = buffer;
    int usableMaxY = gridHeight - buffer - 1;

    if (usableMaxX < usableMinX || usableMaxY < usableMinY)
    {
        return;
    }

    std::vector<ForestSpawnPoint> candidates;

    for (int y = usableMinY; y <= usableMaxY; ++y)
    {
        for (int x = usableMinX; x <= usableMaxX; ++x)
        {
            if (world.getTile(x, y) == TileType::Empty)
            {
                candidates.push_back({x, y});
            }
        }
    }

    if (candidates.empty())
    {
        return;
    }

    std::vector<ForestSpawnPoint> chosenCenters;

    int firstIndex = rand() % candidates.size();
    chosenCenters.push_back(candidates[firstIndex]);
    candidates.erase(candidates.begin() + firstIndex);

    while (static_cast<int>(chosenCenters.size()) < Config::NUM_FORESTS && !candidates.empty())
    {
        double bestDistanceToNearestForest = -1.0;
        int bestCandidateIndex = -1;

        for (int i = 0; i < static_cast<int>(candidates.size()); ++i)
        {
            ForestSpawnPoint candidate = candidates[i];

            double distanceToNearestForest = std::numeric_limits<double>::max();

            for (const auto& center : chosenCenters)
            {
                int dx = candidate.x - center.x;
                int dy = candidate.y - center.y;

                double distanceSquared = dx * dx + dy * dy;

                if (distanceSquared < distanceToNearestForest)
                {
                    distanceToNearestForest = distanceSquared;
                }
            }

            if (distanceToNearestForest > bestDistanceToNearestForest)
            {
                bestDistanceToNearestForest = distanceToNearestForest;
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
        int numForestBlocks = rand() % Config::FOREST_BLOCK_RANGE + Config::MIN_FOREST_BLOCKS;

        Forest forest(center.x, center.y, numForestBlocks, world);

        if (!forest.getCells().empty())
        {
            forests.push_back(forest);
        }
    }

    for (const auto& forest : forests)
    {
        forest.addToWorld(world);
    }
}

/*
    Removes cells that would overwrite lakes, sand, crops, or other non-empty terrain.
*/
void Forest::keepOnlyEmptyWorldCells(const GameWorld& world)
{
    for (auto it = cells.begin(); it != cells.end(); )
    {
        if (!world.isInsideGrid(it->x, it->y) || world.getTile(it->x, it->y) != TileType::Empty)
        {
            it = cells.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

/*
    Writes this forest's cells into the world tile grid.
*/
void Forest::addToWorld(GameWorld& world) const
{
    for (const auto& cell : cells)
    {
        if (world.isInsideGrid(cell.x, cell.y) && world.getTile(cell.x, cell.y) == TileType::Empty)
        {
            world.setTile(cell.x, cell.y, TileType::Forest);
        }
    }
}

/*
    Draws this forest from the world tile grid.
*/
void Forest::draw(GameWorld& world) const
{
    for (const auto& cell : cells)
    {
        if (world.isInsideGrid(cell.x, cell.y) && world.getTile(cell.x, cell.y) == TileType::Forest)
        {
            world.drawTile(cell.x, cell.y, TileType::Forest);
        }
    }
}

/*
    Draws every forest.
*/
void Forest::drawForests(GameWorld& world)
{
    for (const auto& forest : forests)
    {
        forest.draw(world);
    }
}

/*
    Returns whether a world position is a forest tile.
*/
bool Forest::isForestTile(const GameWorld& world, int x, int y)
{
    return world.isInsideGrid(x, y) && world.getTile(x, y) == TileType::Forest;
}
