#include "environment/weather.h"
#include "core/config.h"
#include "environment/daynight.h"
#include "environment/lake.h"
#include "entities/crop.h"
#include "entities/predator.h"

#include <cstdlib>

int Weather::nextWeatherTick = 0;
WeatherEvent Weather::activeEvent = WeatherEvent::None;
int Weather::activeEventTicksRemaining = 0;

/*
    Sets up the first scheduled weather event.
*/
void Weather::init()
{
    scheduleNextWeather();
}

/*
    Updates weather alerts and triggers random weather when its scheduled tick arrives.
*/
void Weather::update(GameWorld& world)
{
    if (activeEventTicksRemaining > 0)
    {
        activeEventTicksRemaining--;
    }

    if (activeEventTicksRemaining <= 0)
    {
        activeEvent = WeatherEvent::None;
    }

    if (DayNight::getTick() >= nextWeatherTick)
    {
        triggerRandomWeather(world);
        scheduleNextWeather();
    }
}

/*
    Starts a flood event.
    Floods clear crops first, then expand all lakes.
*/
void Weather::triggerFlood(GameWorld& world)
{
    Crop::clearAll(world);
    Lake::floodAll(world, Config::WEATHER_FLOOD_LAYERS);

    activeEvent = WeatherEvent::Flood;
    activeEventTicksRemaining = Config::WEATHER_ALERT_TICKS;
}

/*
    Starts a drought event.
    Droughts clear crops, shrink lakes, and kill water predators left off water.
*/
void Weather::triggerDrought(GameWorld& world)
{
    Crop::clearAll(world);
    Lake::dryAll(world, Config::WEATHER_DROUGHT_LAYERS);

    Predator::killWaterPredatorsNotOnWater(world);

    activeEvent = WeatherEvent::Drought;
    activeEventTicksRemaining = Config::WEATHER_ALERT_TICKS;
}

/*
    Returns whether the flood alert should currently be shown.
*/
bool Weather::isFloodAlertActive()
{
    return activeEvent == WeatherEvent::Flood && activeEventTicksRemaining > 0;
}

/*
    Returns whether the drought alert should currently be shown.
*/
bool Weather::isDroughtAlertActive()
{
    return activeEvent == WeatherEvent::Drought && activeEventTicksRemaining > 0;
}

/*
    Randomly chooses which weather event happens next.
*/
void Weather::triggerRandomWeather(GameWorld& world)
{
    int choice = rand() % 2;

    if (choice == 0)
    {
        triggerFlood(world);
    }
    else
    {
        triggerDrought(world);
    }
}

/*
    Picks the next tick when random weather should happen.
    The delay is chosen from the configured day range.
*/
void Weather::scheduleNextWeather()
{
    int minTicks = Config::WEATHER_MIN_DAYS_BETWEEN * Config::TICKS_PER_DAY;
    int maxTicks = Config::WEATHER_MAX_DAYS_BETWEEN * Config::TICKS_PER_DAY;

    int range = maxTicks - minTicks + 1;

    int ticksUntilWeather = minTicks;

    if (range > 0)
    {
        ticksUntilWeather += rand() % range;
    }

    nextWeatherTick = DayNight::getTick() + ticksUntilWeather;
}