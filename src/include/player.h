#pragma once

#include <SFML/Graphics.hpp>
#include "gameworld.h"

class Player
{
public:
    Player(int gridX, int gridY);

    void moveRandomly(GameWorld& world);
    void draw(GameWorld& world) const;

private:
    int x;
    int y;
};