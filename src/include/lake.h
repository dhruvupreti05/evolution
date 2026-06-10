#include <SFML/Graphics.hpp>
#include <set>

#include "block.h"
#include "clumps.h"


class Lake : public Clump {
    public:
        Lake(int centerX, int centerY, int radius);

        void generate();
        void draw(sf::RenderWindow& window) const;

    private:
        int centerX, centerY;
        int radius;
        std::set<Block> water;
        std::set<Block> sand;

        void generateLake();
        void generateBoundary();
};
