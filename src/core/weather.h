#pragma once

#include "core/gameworld.h"

enum class WeatherEvent
{
    None,
    Flood,
    Drought
};

class Weather
{
public:
    static void init();
    static void update(GameWorld& world);

    static void triggerFlood(GameWorld& world);
    static void triggerDrought(GameWorld& world);

    static bool isFloodAlertActive();
    static bool isDroughtAlertActive();

private:
    static int nextWeatherTick;
    static WeatherEvent activeEvent;
    static int activeEventTicksRemaining;

    static void scheduleNextWeather();
    static void triggerRandomWeather(GameWorld& world);
};