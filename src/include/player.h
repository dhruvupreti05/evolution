#pragma once

#include <SFML/Graphics.hpp>

class Player
{
public:
    Player(int gridX, int gridY);

    void moveRandomly();
    void draw(sf::RenderWindow& window) const;

private:
    sf::RectangleShape shape;
    int x;
    int y;
};