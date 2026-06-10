#include "player.h"
#include <cstdlib>

Player::Player(int gridX, int gridY)
    : x(gridX), y(gridY)
{
}

void Player::moveRandomly(GameWorld& world)
{
    int dx = (rand() % 3) - 1;
    int dy = (rand() % 3) - 1;

    int newX = x + dx;
    int newY = y + dy;

    if (!world.isInsideGrid(newX, newY))
    {
        return;
    }

    TileType tile = world.getTile(newX, newY);

    if (tile == TileType::Water)
    {
        return;
    }

    x = newX;
    y = newY;
}

void Player::draw(GameWorld& world) const
{
    world.drawTile(x, y, TileType::Human);
}