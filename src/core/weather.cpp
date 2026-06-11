#include "core/weather.h"

#include "core/config.h"
#include "core/daynight.h"
#include "terrain/lake.h"
#include "resources/crop.h"
#include "entities/predator.h"

#include <cstdlib>

int Weather::nextWeatherTick = 0;
WeatherEvent Weather::activeEvent = WeatherEvent::None;
int Weather::activeEventTicksRemaining = 0;

void Weather::init()
{
    scheduleNextWeather();
}

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

void Weather::triggerFlood(GameWorld& world)
{
    Crop::clearAll(world);
    Lake::floodAll(world, Config::WEATHER_FLOOD_LAYERS);

    activeEvent = WeatherEvent::Flood;
    activeEventTicksRemaining = Config::WEATHER_ALERT_TICKS;
}

void Weather::triggerDrought(GameWorld& world)
{
    Crop::clearAll(world);
    Lake::dryAll(world, Config::WEATHER_DROUGHT_LAYERS);

    Predator::killWaterPredatorsNotOnWater(world);

    activeEvent = WeatherEvent::Drought;
    activeEventTicksRemaining = Config::WEATHER_ALERT_TICKS;
}

bool Weather::isFloodAlertActive()
{
    return activeEvent == WeatherEvent::Flood && activeEventTicksRemaining > 0;
}

bool Weather::isDroughtAlertActive()
{
    return activeEvent == WeatherEvent::Drought && activeEventTicksRemaining > 0;
}

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