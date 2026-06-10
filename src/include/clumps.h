#pragma once

#include <SFML/Graphics.hpp>
#include <set>
#include <vector>

struct GridPos
{
    int x;
    int y;

    bool operator<(const GridPos& other) const
    {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

class Clump
{
public:
    Clump(
        int centerX,
        int centerY,
        int numBlocks,
        int gridSize
    );

    const std::set<GridPos>& getCells() const;

protected:
    std::set<GridPos> cells;

private:
    int centerX;
    int centerY;
    int numBlocks;
    int gridSize;

    void generate();
    std::vector<GridPos> getNeighbors(const GridPos& pos) const;
};