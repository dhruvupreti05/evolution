#include "core/tilecolors.h"

#include "core/config.h"
#include "environment/daynight.h"

sf::Color TileColors::get(TileType tile)
{
    switch (tile)
    {
        case TileType::Water:
            return Config::COLOR_WATER;

        case TileType::Sand:
            return Config::COLOR_SAND;

        case TileType::Tree:
            return Config::COLOR_TREE;

        case TileType::Crop:
            return Config::COLOR_FOOD;

        case TileType::Human:
            return Config::COLOR_HUMAN;

        case TileType::Predator:
            return Config::COLOR_PREDATOR;

        case TileType::Empty:
        default:
            return Config::COLOR_BACKGROUND;
    }
}

sf::Color TileColors::getDayNight(TileType tile)
{
    return DayNight::apply(get(tile));
}