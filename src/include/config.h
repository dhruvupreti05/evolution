#pragma once

#include <SFML/Graphics.hpp>

namespace Config
{
    // Window settings 
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 800;

    constexpr int GRID_SIZE = 100;
    constexpr int CELL_SIZE = 8;

    // Simulation settings
    constexpr int NUM_PLAYERS = 10;
    constexpr int NUM_LAKES = 3;        

    // Colors
    const sf::Color COLOR_BACKGROUND = sf::Color(235, 220, 180); // Light tan
    const sf::Color COLOR_WATER = sf::Color::Blue;
    const sf::Color COLOR_SAND = sf::Color(194, 178, 128); // Dark tan
    const sf::Color COLOR_HUMANS = sf::Color::White; 
}