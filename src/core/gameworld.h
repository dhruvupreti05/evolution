#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "core/tiletype.h"

class GameWorld
{
public:
    GameWorld(
        int windowWidth,
        int windowHeight,
        int gridWidth,
        int gridHeight,
        int cellSize,
        const std::string& title
    );

    bool isOpen() const;
    bool pollEvent(sf::Event& event);

    void clear();
    void display();

    void setTile(int x, int y, TileType type);
    TileType getTile(int x, int y) const;

    bool isInsideGrid(int x, int y) const;

    void drawTile(int x, int y, TileType type);
    void drawTile(int x, int y, sf::Color color);

    void drawTileOutline(int x, int y, sf::Color color, float thickness = 1.0f);

    void drawLine(
        float x1,
        float y1,
        float x2,
        float y2,
        sf::Color color,
        float thickness = 2.0f
    );

    void drawGrid();

    sf::RenderWindow& getWindow();

    int getGridWidth() const;
    int getGridHeight() const;
    int getCellSize() const;

private:
    sf::RenderWindow window;

    int windowWidth;
    int windowHeight;

    int gridWidth;
    int gridHeight;

    int cellSize;

    std::vector<std::vector<TileType>> tiles;

    sf::Color getColorFromTile(TileType type) const;
};