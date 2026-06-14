#pragma once

#include "core/gameworld.h"
#include "core/simulationstate.h"
#include "ui/humaninspector.h"
#include "ui/simulationstats.h"

/*
    Controls the main simulation loop.
*/
class Simulation
{
public:
    Simulation();

    void init(GameWorld& world);
    void handleMainEvent(const sf::Event& event, GameWorld& world);
    void update(GameWorld& world);
    void draw(GameWorld& world);

    bool isPaused() const;

private:
    // Stops the simulation updates while still allowing the world to be drawn.
    bool paused = false;

    HumanInspector inspector;
    SimulationStats statsWindow;

    // Stores the living entities and world-level simulation data.
    SimulationState state;

    void drawWorld(GameWorld& world);
};