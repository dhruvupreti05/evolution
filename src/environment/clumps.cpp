#include "environment/clumps.h"

#include <cstdlib>
#include <queue>
#include <algorithm>

/*
    Creates a random connected clump of grid cells.
    The clump starts from a center cell, grows outward, then fills enclosed holes.
*/
Clump::Clump(int centerX, int centerY, int numBlocks, int gridWidth, int gridHeight) : centerX(centerX), centerY(centerY), numBlocks(numBlocks), gridWidth(gridWidth), gridHeight(gridHeight)
{
    generate();
}

/*
    Returns all cells that belong to this clump.
*/
const std::set<GridPos>& Clump::getCells() const
{
    return cells;
}

/*
    Returns the valid four-direction neighbors of a grid position.
*/
std::vector<GridPos> Clump::getNeighbors(const GridPos& pos) const
{
    std::vector<GridPos> neighbors;

    std::vector<GridPos> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    for (const auto& dir : directions)
    {
        int nx = pos.x + dir.x;
        int ny = pos.y + dir.y;

        if (nx >= 0 && nx < gridWidth && ny >= 0 && ny < gridHeight)
        {
            neighbors.push_back({nx, ny});
        }
    }

    return neighbors;
}

/*
    Grows the clump by repeatedly picking a random edge cell and adding one of its open neighbors.
*/
void Clump::generate()
{
    if (numBlocks <= 0)
    {
        return;
    }

    if (centerX < 0 || centerX >= gridWidth || centerY < 0 || centerY >= gridHeight)
    {
        return;
    }

    cells.insert({centerX, centerY});

    std::vector<GridPos> frontier;
    frontier.push_back({centerX, centerY});

    while (static_cast<int>(cells.size()) < numBlocks && !frontier.empty())
    {
        int index = rand() % frontier.size();
        GridPos current = frontier[index];

        std::vector<GridPos> neighbors = getNeighbors(current);
        std::vector<GridPos> validNeighbors;

        for (const auto& neighbor : neighbors)
        {
            if (cells.count(neighbor) == 0)
            {
                validNeighbors.push_back(neighbor);
            }
        }

        if (validNeighbors.empty())
        {
            // Removes this frontier cell because it has no more unused neighbors to grow into.
            frontier.erase(frontier.begin() + index);
            continue;
        }

        GridPos chosen = validNeighbors[rand() % validNeighbors.size()];

        cells.insert(chosen);
        frontier.push_back(chosen);
    }

    fillHoles();
}

/*
    Fills enclosed empty spaces inside the clump.
    It flood-fills from outside the shape, then treats unreached empty cells as holes.
*/
void Clump::fillHoles()
{
    if (cells.empty())
    {
        return;
    }

    int minX = gridWidth;
    int maxX = 0;
    int minY = gridHeight;
    int maxY = 0;

    for (const auto& cell : cells)
    {
        minX = std::min(minX, cell.x);
        maxX = std::max(maxX, cell.x);
        minY = std::min(minY, cell.y);
        maxY = std::max(maxY, cell.y);
    }

    // Adds a border around the clump so the flood-fill has an outside area to start from.
    minX = std::max(0, minX - 1);
    maxX = std::min(gridWidth - 1, maxX + 1);
    minY = std::max(0, minY - 1);
    maxY = std::min(gridHeight - 1, maxY + 1);

    std::set<GridPos> outside;
    std::queue<GridPos> queue;

    auto tryAddOutside = [&](int x, int y)
    {
        GridPos pos{x, y};

        if (x < minX || x > maxX || y < minY || y > maxY)
        {
            return;
        }

        if (cells.count(pos) > 0)
        {
            return;
        }

        if (outside.count(pos) > 0)
        {
            return;
        }

        outside.insert(pos);
        queue.push(pos);
    };

    for (int x = minX; x <= maxX; ++x)
    {
        tryAddOutside(x, minY);
        tryAddOutside(x, maxY);
    }

    for (int y = minY; y <= maxY; ++y)
    {
        tryAddOutside(minX, y);
        tryAddOutside(maxX, y);
    }

    std::vector<GridPos> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    while (!queue.empty())
    {
        GridPos current = queue.front();
        queue.pop();

        for (const auto& dir : directions)
        {
            int nx = current.x + dir.x;
            int ny = current.y + dir.y;

            tryAddOutside(nx, ny);
        }
    }

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            GridPos pos{x, y};

            bool isClumpCell = cells.count(pos) > 0;
            bool isOutside = outside.count(pos) > 0;

            // Empty cells not connected to the outside are trapped holes, so they become part of the clump.
            if (!isClumpCell && !isOutside)
            {
                cells.insert(pos);
            }
        }
    }
}