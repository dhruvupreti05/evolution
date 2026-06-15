#pragma once

#include <SFML/Graphics.hpp>

#include "entities/visionshape.h"

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
    constexpr int NUM_PLAYERS = 10;
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
    const sf::Color COLOR_DEAD_BODY = sf::Color(120, 120, 120);
    const sf::Color COLOR_PREDATOR = sf::Color::Red;
    const sf::Color COLOR_DEAD_PREDATOR = COLOR_DEAD_BODY;

    // Human settings
    constexpr int HUMAN_START_HEALTH = 1000;
    constexpr int HUMAN_START_THIRST = 1000;
    constexpr int HUMAN_START_HUNGER = 1000;

    constexpr int HUMAN_START_HEALTH_BUFFER = 500;
    constexpr int HUMAN_START_THIRST_BUFFER = 250;
    constexpr int HUMAN_START_HUNGER_BUFFER = 250;

    constexpr int HUMAN_MAX_HEALTH = HUMAN_START_HEALTH + HUMAN_START_HEALTH_BUFFER;
    constexpr int HUMAN_MAX_THIRST = HUMAN_START_THIRST + HUMAN_START_THIRST_BUFFER;
    constexpr int HUMAN_MAX_HUNGER = HUMAN_START_HUNGER + HUMAN_START_HUNGER_BUFFER;

    constexpr int HUMAN_THIRST_DECAY = 2;
    constexpr int HUMAN_HUNGER_DECAY = 1;
    constexpr int HUMAN_AGE_INCREASE = 1;

    // Hunger, thirst, and age change health only on these slower health ticks.
    constexpr int HUMAN_HEALTH_UPDATE_INTERVAL = 10;

    // Values above the safe line do not hurt health. Values below it create damage.
    constexpr int HUMAN_SAFE_HUNGER = 700;
    constexpr int HUMAN_SAFE_THIRST = 700;

    // Excellent food and water can restore health, but age can eventually overpower it.
    constexpr int HUMAN_HEALING_HUNGER = 850;
    constexpr int HUMAN_HEALING_THIRST = 850;
    constexpr int HUMAN_HEALTH_RECOVERY = 3;

    constexpr int HUMAN_HUNGER_DAMAGE_DIVISOR = 100;
    constexpr int HUMAN_THIRST_DAMAGE_DIVISOR = 70;
    constexpr int HUMAN_AGE_DAMAGE_DIVISOR = 4000;

    constexpr int HUMAN_VISION_RANGE = 8;
    constexpr VisionShape HUMAN_VISION_SHAPE = VisionShape::ForwardTriangle;

    constexpr int HUMAN_INVENTORY_SIZE = 5;

    // Inspector window settings
    constexpr int INSPECTOR_WINDOW_WIDTH = 420;
    constexpr int INSPECTOR_WINDOW_HEIGHT = 760;

    constexpr int INSPECTOR_VIEW_TILE_SIZE = 12;
    constexpr int INSPECTOR_VIEW_TOP_MARGIN = 40;
    constexpr int INSPECTOR_STATS_START_Y = 330;

    // Crop settings
    constexpr int FOOD_SPAWN_ATTEMPTS_PER_TICK = 100;
    constexpr int FOOD_SPAWN_CHANCE_PERCENT = 50;

    // Day/night settings
    constexpr int TICKS_PER_DAY = 300;
    constexpr int MIDNIGHT_RGB_DECREASE = 100;

    // Body settings
    constexpr int TICKS_PER_DEAD_BODY = 300;
    constexpr int TICKS_PER_DEAD_PREDATOR = 300;

    // Predator settings

    constexpr int PREDATOR_WATER_SEARCH_RANGE = 20;
    constexpr int PREDATOR_PREY_SEARCH_RANGE = 25;

    constexpr int NUMBER_OF_LAND_PREDATORS = 5;
    constexpr int NUMBER_OF_WATER_PREDATORS = 10;

    constexpr int PREDATOR_START_HEALTH = 1000;
    constexpr int PREDATOR_START_THIRST = 1000;
    constexpr int PREDATOR_START_HUNGER = 1000;

    constexpr int PREDATOR_MAX_HEALTH = 1000;
    constexpr int PREDATOR_MAX_THIRST = 1000;
    constexpr int PREDATOR_MAX_HUNGER = 1000;

    constexpr int PREDATOR_THIRST_DECAY = 2;
    constexpr int PREDATOR_HUNGER_DECAY = 2;

    constexpr int PREDATOR_HEALTH_UPDATE_INTERVAL = 10;
    constexpr int PREDATOR_SAFE_HUNGER = 700;
    constexpr int PREDATOR_SAFE_THIRST = 700;
    constexpr int PREDATOR_HEALING_HUNGER = 850;
    constexpr int PREDATOR_HEALING_THIRST = 850;
    constexpr int PREDATOR_HEALTH_RECOVERY = 2;
    constexpr int PREDATOR_HUNGER_DAMAGE_DIVISOR = 90;
    constexpr int PREDATOR_THIRST_DAMAGE_DIVISOR = 70;

    constexpr int PREDATOR_THIRST_MODE_THRESHOLD = 500;

    constexpr int HEALTH_PER_PREDATOR_ATTACK = 500;

    constexpr int WEATHER_MIN_DAYS_BETWEEN = 1;
    constexpr int WEATHER_MAX_DAYS_BETWEEN = 2;
    constexpr int WEATHER_FLOOD_LAYERS = 1;
    constexpr int WEATHER_DROUGHT_LAYERS = 1;
    constexpr int WEATHER_ALERT_TICKS = 20;

    // Stats window settings
    constexpr int STATS_WINDOW_WIDTH = 560;
    constexpr int STATS_WINDOW_HEIGHT = 430;

    // Crop eating settings
    constexpr int TICKS_PER_MEAL_CROP = 5;
    constexpr int HUNGER_PER_TICK_MEAL_CROP = 20;

    // Body eating settings
    constexpr int HUNGER_PER_TICK_MEAL_HUMAN = 20;
    constexpr int TICKS_PER_MEAL_HUMAN = 10;

    // Water drinking settings
    constexpr int TICKS_PER_WATER_BLOCK_DRINKING = 5;
    constexpr int HUMAN_THIRST_PER_TICK_WATER = 30;
    constexpr int PREDATOR_THIRST_PER_TICK_WATER = 50;

    // Mating settings
    constexpr int HUMAN_MATING_COOLDOWN_TICKS = 20;
    constexpr int PREDATOR_MATING_COOLDOWN_TICKS = 30;
    constexpr int HUMAN_FEMALE_MATING_HEALTH_COST = 150;

}
