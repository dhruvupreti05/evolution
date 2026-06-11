#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>

#include "core/config.h"
#include "core/gameworld.h"
#include "entities/player.h"
#include "terrain/lake.h"
#include "resources/food.h"
#include "ui/playerinspector.h"
#include "core/daynight.h"
#include "entities/predator.h"

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

GameWorld world(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Config::GRID_WIDTH, Config::GRID_HEIGHT, Config::CELL_SIZE, "Evolution Simulation");

    PlayerInspector inspector;

    Lake::init(world);
    Player::init(world);
    Predator::init(world);

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
            }
        }

        if (!paused)
        {
            DayNight::update();

            Player::resetBodyEatingClaims();
            
            Player::updatePlayers(world);
            Predator::updatePredators(world);
            Food::update(world);
        }

        inspector.draw(world);

        world.clear();

        Lake::drawLakes(world);
        Food::drawFoods(world);
        Player::drawBodies(world);
        Player::drawPlayers(world);
        Predator::drawPredators(world);

        world.display();

        sf::sleep(sf::milliseconds(Config::TICK_MS));
    }

    return 0;
}