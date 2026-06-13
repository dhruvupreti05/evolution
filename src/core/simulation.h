#pragma once

#include "core/gameworld.h"
#include "core/simulationstate.h"
#include "ui/humaninspector.h"
#include "ui/simulationstats.h"

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
    bool paused = false;

    HumanInspector inspector;
    SimulationStats statsWindow;
    SimulationState state;

    void drawWorld(GameWorld& world);
};