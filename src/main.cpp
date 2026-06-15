#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "core/config.h"
#include "core/gameworld.h"
#include "core/simulation.h"

/*
    Program entry point.
    Sets up the world, starts the simulation, and runs the main update/draw loop.
*/
int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    GameWorld world(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Config::GRID_WIDTH, Config::GRID_HEIGHT, Config::CELL_SIZE, "Evolution Simulation");

    Simulation simulation;
    simulation.init(world);

    while (world.isOpen())
    {
        sf::Clock tickClock;

        sf::Event event;

        while (world.pollEvent(event))
        {
            simulation.handleMainEvent(event, world);
        }

        simulation.update(world);
        simulation.draw(world);

        sf::Time elapsed = tickClock.getElapsedTime();
        sf::Time targetTickTime = sf::milliseconds(Config::TICK_MS);

        // Sleeps only for the remaining time, so slow update/draw work is not added on top of TICK_MS.
        if (elapsed < targetTickTime)
        {
            sf::sleep(targetTickTime - elapsed);
        }

        std::cout << "Tick work time: " << elapsed.asMilliseconds() << " ms" << std::endl;
    }

    return 0;
}