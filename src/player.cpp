#include "Player.h"
#include <cstdlib>

Player::Player(int gridX, int gridY) 
{
    x = gridX;
    y = gridY;
    shape.setSize(sf::Vector2f(8, 8));  
    shape.setFillColor(sf::Color::White);
    shape.setPosition(x * 8, y * 8); 
}

void Player::moveRandomly() 
{
    int dx = (rand() % 3) - 1;
    int dy = (rand() % 3) - 1;
    x = (x + dx + 100) % 100;
    y = (y + dy + 100) % 100;
    shape.setPosition(x * 8, y * 8);
}
