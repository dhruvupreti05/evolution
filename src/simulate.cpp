#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>

#include "core/config.h"
#include "core/gameworld.h"
#include "entities/human.h"
#include "environment/lake.h"
#include "entities/crop.h"
#include "ui/humaninspector.h"
#include "environment/daynight.h"
#include "entities/predator.h"
#include "environment/weather.h"
#include "ui/simulationstats.h"

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

GameWorld world(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Config::GRID_WIDTH, Config::GRID_HEIGHT, Config::CELL_SIZE, "Evolution Simulation");

    HumanInspector inspector;
    SimulationStats statsWindow;

    Lake::init(world);
    Human::init(world);
    Predator::init(world);
    Weather::init();

    bool paused = false;

    while (world.isOpen())
    {
        sf::Event event;

        while (world.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                world.getWindow().close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
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
                }

                if (event.key.code == sf::Keyboard::G)
                {
                    Weather::triggerDrought(world);
                }
                if (event.key.code == sf::Keyboard::T)
                {
                    statsWindow.open();
                }
            }
        }

        if (!paused)
        {
            DayNight::update();

            Human::resetBodyEatingClaims();

            Human::updateHumans(world);
            Predator::updatePredators(world);
            Crop::update(world);

            Weather::update(world);
        }
        inspector.draw(world);
        statsWindow.draw(world);

        world.clear();

        Lake::drawLakes(world);
        Crop::drawCrops(world);

        Human::drawBodies(world);
        Predator::drawBodies(world);

        Human::drawHumans(world);
        Predator::drawPredators(world);

        world.display();

        sf::sleep(sf::milliseconds(Config::TICK_MS));
    }

    return 0;
}