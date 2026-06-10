#pragma once

#include <SFML/Graphics.hpp>

namespace Config
{
    // Window settings
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 800;

    // Grid settings
    constexpr int GRID_SIZE = 100;
    constexpr int CELL_SIZE = 8;

    // Simulation settings
    constexpr int NUM_PLAYERS = 100;
    constexpr int NUM_LAKES = 7;

    // Time settings
    constexpr int TICK_MS = 100;

    // Lake settings
    constexpr int MIN_LAKE_BLOCKS = 100;
    constexpr int LAKE_BLOCK_RANGE = 50;

    // Colors
    const sf::Color COLOR_BACKGROUND = sf::Color(235, 220, 180);
    const sf::Color COLOR_WATER = sf::Color::Blue;
    const sf::Color COLOR_SAND = sf::Color(194, 178, 128);
    const sf::Color COLOR_HUMAN = sf::Color::White;
    const sf::Color COLOR_TREE = sf::Color(34, 139, 34);
    const sf::Color COLOR_FOOD = sf::Color::Red;
}