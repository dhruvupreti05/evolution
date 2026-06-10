#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>

#include "config.h"
#include "gameworld.h"
#include "player.h"
#include "lake.h"

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    GameWorld world(
        Config::WINDOW_WIDTH,
        Config::WINDOW_HEIGHT,
        Config::GRID_SIZE,
        Config::CELL_SIZE,
        "Evolution Simulation"
    );

    Player::init(world);
    Lake::init(world);

    while (world.isOpen())
    {
        sf::Event event;

        while (world.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                world.getWindow().close();
            }
        }

        Player::movePlayers(world);

        world.clear();

        Lake::drawLakes(world);
        Player::drawPlayers(world);

        world.display();

        sf::sleep(sf::milliseconds(Config::TICK_MS));
    }

    return 0;
}