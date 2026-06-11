#pragma once

#include <SFML/Graphics.hpp>

namespace Config
{
    // Window settings
    constexpr int WINDOW_WIDTH = 1200;
    constexpr int WINDOW_HEIGHT = 800;

    // Grid settings
    constexpr int CELL_SIZE = 8;
    constexpr int GRID_WIDTH = WINDOW_WIDTH / CELL_SIZE; 
    constexpr int GRID_HEIGHT = WINDOW_HEIGHT / CELL_SIZE;

    // Simulation settings
    constexpr int NUM_PLAYERS = 100;
    constexpr int NUM_LAKES = 10;

    // Time settings
    constexpr int TICK_MS = 250;

    // Lake settings
    constexpr int MIN_LAKE_BLOCKS = 250;
    constexpr int LAKE_BLOCK_RANGE = 50;
    constexpr int LAKE_SPAWN_BOUNDARY_BUFFER = 10;

    // Colors
    const sf::Color COLOR_BACKGROUND = sf::Color(235, 220, 180);
    const sf::Color COLOR_WATER = sf::Color::Blue;
    const sf::Color COLOR_SAND = sf::Color(194, 178, 128);
    const sf::Color COLOR_HUMAN = sf::Color::White;
    const sf::Color COLOR_TREE = sf::Color(34, 139, 34);
    const sf::Color COLOR_FOOD = sf::Color::Green;
    const sf::Color COLOR_SELECTED_HUMAN = sf::Color::Black;

    // Human settings
    constexpr int HUMAN_START_HEALTH = 1000;
    constexpr int HUMAN_START_THIRST = 1000;
    constexpr int HUMAN_START_HUNGER = 1000;

    constexpr int HUMAN_START_HEALTH_BUFFER = 500;
    constexpr int HUMAN_START_THIRST_BUFFER = 250;
    constexpr int HUMAN_START_HUNGER_BUFFER = 250;

    constexpr int HUMAN_HEALTH_DECAY = 0;
    constexpr int HUMAN_THIRST_DECAY = 2;
    constexpr int HUMAN_HUNGER_DECAY = 1;
    constexpr int HUMAN_AGE_INCREASE = 1;

    constexpr int HUMAN_VISION_RANGE = 10; // after 11 its too big for window view
    constexpr int HUMAN_INVENTORY_SIZE = 5;

    // Inspector window settings
    constexpr int INSPECTOR_WINDOW_WIDTH = 420;
    constexpr int INSPECTOR_WINDOW_HEIGHT = 760;

    constexpr int INSPECTOR_VIEW_TILE_SIZE = 18;
    constexpr int INSPECTOR_VIEW_TOP_MARGIN = 40;
    constexpr int INSPECTOR_STATS_START_Y = 330;

    // Food settings
    constexpr int FOOD_SPAWN_ATTEMPTS_PER_TICK = 100;
    constexpr int FOOD_SPAWN_CHANCE_PERCENT = 50;

    // Day/night settings
    constexpr int TICKS_PER_DAY = 300;
    constexpr int MIDNIGHT_RGB_DECREASE = 150;

}