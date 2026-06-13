#include "core/gridutils.h"

#include <cstdlib>
#include <cmath>

int GridUtils::manhattanDistance(int x1, int y1, int x2, int y2)
{
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

bool GridUtils::isFourNeighborDistance(int x1, int y1, int x2, int y2)
{
    return manhattanDistance(x1, y1, x2, y2) == 1;
}

Direction GridUtils::randomDirection()
{
    int choice = rand() % 5;

    switch (choice)
    {
        case 1:
            return Direction::Up;

        case 2:
            return Direction::Down;

        case 3:
            return Direction::Left;

        case 4:
            return Direction::Right;

        case 0:
        default:
            return Direction::Stay;
    }
}

Direction GridUtils::directionToward(
    int fromX,
    int fromY,
    int targetX,
    int targetY
)
{
    if (targetX < fromX)
    {
        return Direction::Left;
    }

    if (targetX > fromX)
    {
        return Direction::Right;
    }

    if (targetY < fromY)
    {
        return Direction::Up;
    }

    if (targetY > fromY)
    {
        return Direction::Down;
    }

    return Direction::Stay;
}

void GridUtils::directionToDelta(Direction direction, int& dx, int& dy)
{
    dx = 0;
    dy = 0;

    switch (direction)
    {
        case Direction::Up:
            dy = -1;
            break;

        case Direction::Down:
            dy = 1;
            break;

        case Direction::Left:
            dx = -1;
            break;

        case Direction::Right:
            dx = 1;
            break;

        case Direction::Stay:
        default:
            break;
    }
}

void GridUtils::getNeighbor(
    int x,
    int y,
    Direction direction,
    int& outX,
    int& outY
)
{
    int dx;
    int dy;

    directionToDelta(direction, dx, dy);

    outX = x + dx;
    outY = y + dy;
}