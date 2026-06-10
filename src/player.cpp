#include "player.h"
#include <cstdlib>

const int GRID_SIZE = 100;
const int CELL_SIZE = 8;

Player::Player(int gridX, int gridY)
{
    x = gridX;
    y = gridY;

    shape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
    shape.setFillColor(sf::Color::White);
    shape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
}

void Player::moveRandomly()
{
    int dx = (rand() % 3) - 1;
    int dy = (rand() % 3) - 1;

    x = (x + dx + GRID_SIZE) % GRID_SIZE;
    y = (y + dy + GRID_SIZE) % GRID_SIZE;

    shape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
}

void Player::draw(sf::RenderWindow& window) const
{
    window.draw(shape);
}