#include "clumps.h"
#include <cstdlib>
#include <vector>

Clump::Clump(int centerX, int centerY, int numBlocks, int gridSize)
    : centerX(centerX), centerY(centerY), numBlocks(numBlocks), gridSize(gridSize)
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
    cells.insert({centerX, centerY});

    std::vector<GridPos> frontier;
    frontier.push_back({centerX, centerY});

    while ((int)cells.size() < numBlocks && !frontier.empty())
    {
        int index = rand() % frontier.size();
        GridPos current = frontier[index];

        std::vector<GridPos> neighbors = getNeighbors(current);

        if (neighbors.empty())
        {
            frontier.erase(frontier.begin() + index);
            continue;
        }

        GridPos chosen = neighbors[rand() % neighbors.size()];

        if (cells.count(chosen) == 0)
        {
            cells.insert(chosen);
            frontier.push_back(chosen);
        }
    }
}