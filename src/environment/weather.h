#pragma once

#include "core/gameworld.h"

/*
    Weather events that can temporarily change the world.
    None means no active event is currently happening.
*/
enum class WeatherEvent
{
    None,
    Flood,
    Drought
};

/*
    Controls random and manual weather events.
    Weather can flood lakes, dry lakes, and expose alerts while an event is active.
*/
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
    // Tick when the next random weather event should happen.
    static int nextWeatherTick;

    static WeatherEvent activeEvent;

    // Keeps the flood/drought alert visible for a few ticks after triggering.
    static int activeEventTicksRemaining;

    static void scheduleNextWeather();
    static void triggerRandomWeather(GameWorld& world);
};