#include "core/config.h"
#include "core/debuglog.h"
#include "core/simulation.h"
#include "entities/crop.h"
#include "entities/entityoccupancy.h"
#include "entities/human.h"
#include "entities/body.h"
#include "entities/predator.h"
#include "environment/daynight.h"
#include "environment/forest.h"
#include "environment/lake.h"
#include "environment/weather.h"

Simulation::Simulation()
{
}

/*
    Sets up the starting world state.
*/
void Simulation::init(GameWorld& world)
{
    Lake::init(world);
    Forest::init(world);
    Human::init(world);
    Predator::init(world);
    Weather::init();

    EntityOccupancy::rebuild(world);

    DebugLog::setEnabled(false);
}

/*
    Handles keyboard input for the main window.
*/
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

        // Weather can change blocked/open tiles, so occupancy needs to match the new world.
        EntityOccupancy::rebuild(world);
    }

    if (event.key.code == sf::Keyboard::G)
    {
        Weather::triggerDrought(world);

        // Same reason as flooding: terrain changes can affect where entities can be.
        EntityOccupancy::rebuild(world);
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

/*
    Advances the simulation by one step.
    Note that the order matters because entities, crops, weather, and occupancy all depend on each other.
*/
void Simulation::update(GameWorld& world)
{
    if (paused)
    {
        return;
    }

    DayNight::update();
    Body::resetEatingClaims();

    EntityOccupancy::rebuild(world);

    Human::updateHumans(world);
    Predator::updatePredators(world);

    // Rebuild after movement and deaths so crops/weather see the current entity positions.
    EntityOccupancy::rebuild(world);

    Crop::update(world);
    Weather::update(world);
    Body::update(world);

    // Final rebuild keeps occupancy correct for drawing, inspectors, and the next frame.
    EntityOccupancy::rebuild(world);
}

/*
    Draws the main world and any open side windows.
*/
void Simulation::draw(GameWorld& world)
{
    inspector.draw(world);
    statsWindow.draw(world);

    drawWorld(world);
}

/*
    Returns whether simulation updates are currently stopped.
*/
bool Simulation::isPaused() const
{
    return paused;
}

/*
    Draws the visible world in layer order.
    Terrain is drawn first, then dead bodies, then living entities on top.
*/
void Simulation::drawWorld(GameWorld& world)
{
    world.clear();

    Lake::drawLakes(world);
    Forest::drawForests(world);
    Crop::drawCrops(world);

    Body::drawBodies(world);

    Human::drawHumans(world);
    Predator::drawPredators(world);

    world.display();
}
