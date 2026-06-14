#pragma once

#include "entities/direction.h"

/*
    Helper functions for working with grid movement and distances.
*/
namespace GridUtils
{
    /*
        Stores how much a position changes in x and y for one step.
    */
    struct Delta
    {
        int dx;
        int dy;
    };

    // The four directions that do not include diagonals.
    constexpr Delta FOUR_DIRECTIONS[4] = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1}
    };

    // All neighboring directions, including diagonals.
    constexpr Delta EIGHT_DIRECTIONS[8] = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1},
        {1, 1},
        {1, -1},
        {-1, 1},
        {-1, -1}
    };

    int manhattanDistance(int x1, int y1, int x2, int y2);
    bool isFourNeighborDistance(int x1, int y1, int x2, int y2);

    Direction randomDirection();
    Direction directionToward(int fromX, int fromY, int targetX, int targetY);

    void directionToDelta(Direction direction, int& dx, int& dy);
    void getNeighbor(int x, int y, Direction direction, int& outX, int& outY);
}