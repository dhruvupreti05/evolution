#include "core/daynight.h"
#include "core/config.h"

#include <algorithm>

int DayNight::tick = 0;

void DayNight::update()
{
    tick++;
}

int DayNight::getTick()
{
    return tick;
}

int DayNight::getTimeOfDay()
{
    return tick % Config::TICKS_PER_DAY;
}

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
        return static_cast<int>(
            static_cast<float>(Config::MIDNIGHT_RGB_DECREASE) *
            static_cast<float>(timeOfDay) /
            static_cast<float>(halfDay)
        );
    }

    return static_cast<int>(
        static_cast<float>(Config::MIDNIGHT_RGB_DECREASE) *
        static_cast<float>(Config::TICKS_PER_DAY - timeOfDay) /
        static_cast<float>(halfDay)
    );
}

int DayNight::clampColorValue(int value)
{
    return std::max(0, std::min(255, value));
}

sf::Color DayNight::apply(sf::Color color)
{
    int darkness = getDarknessAmount();

    return sf::Color(
        clampColorValue(static_cast<int>(color.r) - darkness),
        clampColorValue(static_cast<int>(color.g) - darkness),
        clampColorValue(static_cast<int>(color.b) - darkness),
        color.a
    );
}