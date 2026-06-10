#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "tiletype.h"

class GameWorld
{
public:
    GameWorld(int windowWidth, int windowHeight, int gridSize, int cellSize, const std::string& title);

    bool isOpen() const;
    bool pollEvent(sf::Event& event);

    void clear();
    void display();

    void setTile(int x, int y, TileType type);
    TileType getTile(int x, int y) const;

    bool isInsideGrid(int x, int y) const;

    void drawTile(int x, int y, TileType type);
    void drawGrid();

    sf::RenderWindow& getWindow();

    int getGridSize() const;
    int getCellSize() const;

private:
    sf::RenderWindow window;

    int windowWidth;
    int windowHeight;
    int gridSize;
    int cellSize;

    std::vector<std::vector<TileType>> tiles;

    sf::Color getColorFromTile(TileType type) const;
};