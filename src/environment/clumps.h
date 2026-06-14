#pragma once

#include <set>
#include <vector>

/*
    Grid coordinate used inside clump generation.
    The less-than operator lets GridPos be stored in std::set.
*/
struct GridPos
{
    int x;
    int y;

    bool operator<(const GridPos& other) const
    {
        if (x != other.x)
        {
            return x < other.x;
        }

        return y < other.y;
    }
};

/*
    Generates a connected group of grid cells.
    Lakes and other terrain patches can use this to form natural-looking blobs instead of perfect rectangles.
*/
class Clump
{
public:
    Clump(int centerX, int centerY, int numBlocks, int gridWidth, int gridHeight);

    const std::set<GridPos>& getCells() const;

protected:
    // Stores every grid cell that belongs to this clump.
    std::set<GridPos> cells;

    int gridWidth;
    int gridHeight;

private:
    int centerX;
    int centerY;
    int numBlocks;

    void generate();

    // Fills small gaps inside the generated shape so the clump looks less scattered.
    void fillHoles();

    std::vector<GridPos> getNeighbors(const GridPos& pos) const;
};