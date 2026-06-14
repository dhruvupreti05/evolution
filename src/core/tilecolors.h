#pragma once

#include <SFML/Graphics.hpp>

#include "core/tiletype.h"

/*
    Gives each tile type its drawing color.
    It can return either the normal color or the color adjusted for the day/night cycle.
*/
namespace TileColors
{
    sf::Color get(TileType tile);
    sf::Color getDayNight(TileType tile);
}