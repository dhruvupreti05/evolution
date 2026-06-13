#pragma once

#include <SFML/Graphics.hpp>

#include "core/tiletype.h"

namespace TileColors
{
    sf::Color get(TileType tile);
    sf::Color getDayNight(TileType tile);
}