

#include <SFML/Graphics.hpp>

class Player {
    sf::RectangleShape shape;
    int x, y;

    Player(int gridX, int gridY);
    void moveRandomly();
};