#include <SFML/Graphics.hpp>
#include "block.h"

Block::Block(int x, int y, sf::Color color) : x(x), y(y), color(color)
{
    size = 8;
    block = new sf::RectangleShape(sf::Vector2f(size, size));
    block.setFillColor(color);
    block.setPosition((x - gridX) * size, (y - gridY) * size); 
}

void Block::draw(sf::RenderWindow& window) const
{
    window.draw(block);
}