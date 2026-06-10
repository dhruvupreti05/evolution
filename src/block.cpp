#include "block.h"

Block::Block(int gridX, int gridY, int size, sf::Color color) : gridX(gridX), gridY(gridY), size(size)
{
    shape.setSize(sf::Vector2f(size, size));
    shape.setFillColor(color);
    shape.setPosition(gridX * size, gridY * size);
}

void Block::draw(sf::RenderWindow& window) const
{
    window.draw(shape);
}