#pragma once

#include <SFML/Graphics.hpp>

/*
    Tracks the simulation's day/night cycle.
    It darkens colors based on the current time so the world visually changes over time.
*/
class DayNight
{
public:
    static void update();

    static int getTick();
    static int getTimeOfDay();
    static int getDarknessAmount();

    static sf::Color apply(sf::Color color);

private:
    static int tick;

    // Keeps color values inside the valid 0-255 range after darkening.
    static int clampColorValue(int value);
};