#include "environment/daynight.h"
#include "core/config.h"

#include <algorithm>

int DayNight::tick = 0;

/*
    Advances the day/night clock by one simulation tick.
*/
void DayNight::update()
{
    tick++;
}

/*
    Returns the total number of ticks since the simulation started.
*/
int DayNight::getTick()
{
    return tick;
}

/*
    Returns the current position within the repeating day cycle.
*/
int DayNight::getTimeOfDay()
{
    return tick % Config::TICKS_PER_DAY;
}

/*
    Calculates how much darker the world should be right now.
    Darkness rises until halfway through the day cycle, then fades back down.
*/
int DayNight::getDarknessAmount()
{
    int timeOfDay = getTimeOfDay();

    int halfDay = Config::TICKS_PER_DAY / 2;

    if (halfDay <= 0)
    {
        return 0;
    }

    if (timeOfDay <= halfDay)
    {
        return static_cast<int>(static_cast<float>(Config::MIDNIGHT_RGB_DECREASE) * static_cast<float>(timeOfDay) / static_cast<float>(halfDay));
    }

    return static_cast<int>(static_cast<float>(Config::MIDNIGHT_RGB_DECREASE) * static_cast<float>(Config::TICKS_PER_DAY - timeOfDay) / static_cast<float>(halfDay));
}

/*
    Keeps an RGB value inside SFML's valid color range.
*/
int DayNight::clampColorValue(int value)
{
    return std::max(0, std::min(255, value));
}

/*
    Applies the current darkness level to a color.
    Alpha is left unchanged so transparency still works normally.
*/
sf::Color DayNight::apply(sf::Color color)
{
    int darkness = getDarknessAmount();

    return sf::Color(clampColorValue(static_cast<int>(color.r) - darkness), clampColorValue(static_cast<int>(color.g) - darkness), clampColorValue(static_cast<int>(color.b) - darkness), color.a);
}