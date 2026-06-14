#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>

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
        sf::Event event;

        while (world.pollEvent(event))
        {
            simulation.handleMainEvent(event, world);
        }

        simulation.update(world);
        simulation.draw(world);

        // Keeps the simulation from running as fast as the CPU allows.
        sf::sleep(sf::milliseconds(Config::TICK_MS));
    }

    return 0;
}