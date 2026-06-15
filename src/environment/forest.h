#pragma once

#include <vector>

#include "environment/clumps.h"
#include "core/gameworld.h"

/*
    Forest terrain feature built from clumps of forest cells.
    Forests do not block movement, but they hide humans and bodies from predator search.
*/
class Forest : public Clump
{
public:
    Forest(int centerX, int centerY, int numForestBlocks, const GameWorld& world);

    static void init(GameWorld& world);
    static void drawForests(GameWorld& world);

    void addToWorld(GameWorld& world) const;
    void draw(GameWorld& world) const;

    static bool isForestTile(const GameWorld& world, int x, int y);

private:
    static std::vector<Forest> forests;

    // Removes generated clump cells that would overlap lakes, sand, crops, or any other terrain.
    void keepOnlyEmptyWorldCells(const GameWorld& world);
};
