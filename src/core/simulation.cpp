#include "core/simulation.h"

#include "core/config.h"
#include "core/debuglog.h"
#include "entities/crop.h"
#include "entities/entityoccupancy.h"
#include "entities/human.h"
#include "entities/predator.h"
#include "environment/daynight.h"
#include "environment/lake.h"
#include "environment/weather.h"

Simulation::Simulation()
{
}

void Simulation::init(GameWorld& world)
{
    Lake::init(world);
    Human::init(world);
    Predator::init(world);
    Weather::init();

    EntityOccupancy::rebuild();

    DebugLog::setEnabled(false);
}

void Simulation::handleMainEvent(const sf::Event& event, GameWorld& world)
{
    if (event.type == sf::Event::Closed)
    {
        world.getWindow().close();
        return;
    }

    if (event.type != sf::Event::KeyPressed)
    {
        return;
    }

    if (event.key.code == sf::Keyboard::Q)
    {
        inspector.open();
    }

    if (event.key.code == sf::Keyboard::P)
    {
        paused = !paused;
    }

    if (event.key.code == sf::Keyboard::F)
    {
        Weather::triggerFlood(world);
        EntityOccupancy::rebuild();
    }

    if (event.key.code == sf::Keyboard::G)
    {
        Weather::triggerDrought(world);
        EntityOccupancy::rebuild();
    }

    if (event.key.code == sf::Keyboard::T)
    {
        statsWindow.open();
    }

    if (event.key.code == sf::Keyboard::L)
    {
        DebugLog::setEnabled(!DebugLog::isEnabled());
    }
}

void Simulation::update(GameWorld& world)
{
    if (paused)
    {
        return;
    }

    DayNight::update();
    Human::resetBodyEatingClaims();

    EntityOccupancy::rebuild();

    Human::updateHumans(world);
    Predator::updatePredators(world);

    EntityOccupancy::rebuild();

    Crop::update(world);
    Weather::update(world);

    EntityOccupancy::rebuild();
}

void Simulation::draw(GameWorld& world)
{
    inspector.draw(world);
    statsWindow.draw(world);

    drawWorld(world);
}

bool Simulation::isPaused() const
{
    return paused;
}

void Simulation::drawWorld(GameWorld& world)
{
    world.clear();

    Lake::drawLakes(world);
    Crop::drawCrops(world);

    Human::drawBodies(world);
    Predator::drawBodies(world);

    Human::drawHumans(world);
    Predator::drawPredators(world);

    world.display();
}