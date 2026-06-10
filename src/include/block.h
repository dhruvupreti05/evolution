#pragma once

#include <SFML/Graphics.hpp>

class Block
{
public:
    Block(int gridX, int gridY, int size, sf::Color color);

    void draw(sf::RenderWindow& window) const;

private:
    int gridX;
    int gridY;
    int size;
    sf::RectangleShape shape;
};