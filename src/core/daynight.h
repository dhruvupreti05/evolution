#pragma once

#include <SFML/Graphics.hpp>

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

    static int clampColorValue(int value);
};