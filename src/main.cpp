#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "gameworld.h"
#include "player.h"
#include "lake.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int GRID_SIZE = 100;
const int CELL_SIZE = 8;

const int NUM_PLAYERS = 10;
const int NUM_LAKES = 3;

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    GameWorld world(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        GRID_SIZE,
        CELL_SIZE,
        "Evolution Simulation"
    );

    std::vector<Player> players;
    std::vector<Lake> lakes;

    // Create players once
    for (int i = 0; i < NUM_PLAYERS; ++i)
    {
        int gridX = rand() % GRID_SIZE;
        int gridY = rand() % GRID_SIZE;

        players.emplace_back(gridX, gridY);
    }

    // Create lakes once
    for (int i = 0; i < NUM_LAKES; ++i)
    {
        int gridX = rand() % GRID_SIZE;
        int gridY = rand() % GRID_SIZE;
        int numWaterBlocks = rand() % 50 + 30;

        lakes.emplace_back(
            gridX,
            gridY,
            numWaterBlocks,
            GRID_SIZE,
            CELL_SIZE
        );
    }

    // Write lakes into the world's TileType grid once
    for (const auto& lake : lakes)
    {
        lake.addToWorld(world);
    }

    // Main game loop
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

        // Update players
        for (auto& player : players)
        {
            player.moveRandomly(world);
        }

        // Draw everything
        world.clear();

        for (const auto& lake : lakes)
        {
            lake.draw(world);
        }

        for (const auto& player : players)
        {
            player.draw(world);
        }

        world.display();

        sf::sleep(sf::milliseconds(100));
    }

    return 0;
}