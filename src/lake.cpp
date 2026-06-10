#include "lake.h"
#include <cstdlib>

Lake::Lake(int centerX, int centerY, int radius)
{
    this.centerX = centerX;
    this.centerY = centerY;
    this.radius = radius;
    generate();
}

void Lake::generate() 
{
    generateLake();
    generateBoundary();
}

void Lake::generateLake() {
    if (radius <= 0) return;

    for (int iy = radius; iy >= -radius; iy--)
    {
        for (int ix = )
    }

}

void Lake::draw(sf::RenderWindow& window) const 
{
    for (const auto& block : blocks) 
    {
        window.draw(block);
    }
}