#include <SFML/Graphics.hpp>

class Block
{
    public:
        Block(int x, int y, int size, sf::Color color);
        void draw(sf::RenderWindow& window) const;
    
    private:
        int x, y;
        int size;
        sf::RectangleShape block;
        sf::Color color;
};