#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "player.h"
#include "lake.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int GRID_SIZE = 100;
const int CELL_SIZE = 8;

const int NUM_PLAYERS = 10;
const int NUM_LAKES = 3;

void drawGrid(sf::RenderWindow& window)
{
    sf::VertexArray verticalLines(sf::Lines);
    sf::VertexArray horizontalLines(sf::Lines);

    for (int i = 0; i <= GRID_SIZE; ++i)
    {
        verticalLines.append(
            sf::Vertex(
                sf::Vector2f(i * CELL_SIZE, 0),
                sf::Color::White
            )
        );

        verticalLines.append(
            sf::Vertex(
                sf::Vector2f(i * CELL_SIZE, WINDOW_HEIGHT),
                sf::Color::White
            )
        );
    }

    for (int i = 0; i <= GRID_SIZE; ++i)
    {
        horizontalLines.append(
            sf::Vertex(
                sf::Vector2f(0, i * CELL_SIZE),
                sf::Color::White
            )
        );

        horizontalLines.append(
            sf::Vertex(
                sf::Vector2f(WINDOW_WIDTH, i * CELL_SIZE),
                sf::Color::White
            )
        );
    }

    window.draw(verticalLines);
    window.draw(horizontalLines);
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    sf::RenderWindow window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
        "Evolution Simulation"
    );

    std::vector<Player> players;
    std::vector<Lake> lakes;

    for (int i = 0; i < NUM_PLAYERS; ++i)
    {
        int gridX = rand() % GRID_SIZE;
        int gridY = rand() % GRID_SIZE;

        players.emplace_back(gridX, gridY);
    }

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

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        for (auto& player : players)
        {
            player.moveRandomly();
        }

        window.clear(sf::Color(235, 220, 180));

        // Optional:
        // drawGrid(window);

        for (const auto& lake : lakes)
        {
            lake.draw(window);
        }

        for (const auto& player : players)
        {
            player.draw(window);
        }

        window.display();

        sf::sleep(sf::milliseconds(100));
    }

    return 0;
}