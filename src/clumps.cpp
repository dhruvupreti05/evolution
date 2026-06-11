#include "clumps.h"

#include <cstdlib>
#include <queue>
#include <algorithm>

Clump::Clump(int centerX, int centerY, int numBlocks, int gridSize)
    : centerX(centerX),
      centerY(centerY),
      numBlocks(numBlocks),
      gridSize(gridSize)
{
    generate();
}

const std::set<GridPos>& Clump::getCells() const
{
    return cells;
}

std::vector<GridPos> Clump::getNeighbors(const GridPos& pos) const
{
    std::vector<GridPos> neighbors;

    std::vector<GridPos> directions = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1}
    };

    for (const auto& dir : directions)
    {
        int nx = pos.x + dir.x;
        int ny = pos.y + dir.y;

        if (nx >= 0 && nx < gridSize && ny >= 0 && ny < gridSize)
        {
            neighbors.push_back({nx, ny});
        }
    }

    return neighbors;
}

void Clump::generate()
{
    if (numBlocks <= 0)
    {
        return;
    }

    if (centerX < 0 || centerX >= gridSize || centerY < 0 || centerY >= gridSize)
    {
        return;
    }

    cells.insert({centerX, centerY});

    std::vector<GridPos> frontier;
    frontier.push_back({centerX, centerY});

    while ((int)cells.size() < numBlocks && !frontier.empty())
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
            frontier.erase(frontier.begin() + index);
            continue;
        }

        GridPos chosen = validNeighbors[rand() % validNeighbors.size()];

        cells.insert(chosen);
        frontier.push_back(chosen);
    }

    fillHoles();
}

void Clump::fillHoles()
{
    if (cells.empty())
    {
        return;
    }

    int minX = gridSize;
    int maxX = 0;
    int minY = gridSize;
    int maxY = 0;

    for (const auto& cell : cells)
    {
        minX = std::min(minX, cell.x);
        maxX = std::max(maxX, cell.x);
        minY = std::min(minY, cell.y);
        maxY = std::max(maxY, cell.y);
    }

    // Add a 1-cell margin around the bounding box.
    // This gives the flood-fill an "outside" area to start from.
    minX = std::max(0, minX - 1);
    maxX = std::min(gridSize - 1, maxX + 1);
    minY = std::max(0, minY - 1);
    maxY = std::min(gridSize - 1, maxY + 1);

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

    // Start flood-fill from the edges of the bounding box.
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

    std::vector<GridPos> directions = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1}
    };

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

    // Any empty cell inside the bounding box that was NOT reached
    // by the outside flood-fill is an enclosed hole.
    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            GridPos pos{x, y};

            bool isWater = cells.count(pos) > 0;
            bool isOutside = outside.count(pos) > 0;

            if (!isWater && !isOutside)
            {
                cells.insert(pos);
            }
        }
    }
}