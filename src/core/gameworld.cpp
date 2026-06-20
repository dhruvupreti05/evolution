#include "core/gameworld.h"
#include "environment/daynight.h"
#include "entities/body.h"
#include "core/config.h"
#include "core/tilecolors.h"

#include <cmath>

/*
    Creates the SFML window and builds the tile grid.
    Every tile starts as empty until the world setup changes it.
*/
GameWorld::GameWorld(int windowWidth, int windowHeight, int gridWidth, int gridHeight, int cellSize, const std::string& title) : window(sf::VideoMode(windowWidth, windowHeight), title), windowWidth(windowWidth), windowHeight(windowHeight), gridWidth(gridWidth), gridHeight(gridHeight), cellSize(cellSize), tiles(gridHeight, std::vector<TileType>(gridWidth, TileType::Empty))
{
}

/*
    Returns whether the game window is still open.
*/
bool GameWorld::isOpen() const
{
    return window.isOpen();
}

/*
    Pulls the next SFML event from the window.
*/
bool GameWorld::pollEvent(sf::Event& event)
{
    return window.pollEvent(event);
}

/*
    Clears the window using the current day/night-adjusted background color.
*/
void GameWorld::clear()
{
    window.clear(DayNight::apply(Config::COLOR_BACKGROUND));
}

/*
    Shows everything that was drawn this frame.
*/
void GameWorld::display()
{
    window.display();
}

/*
    Changes the tile type at a grid position.
    Invalid positions are ignored so outside-grid writes do not crash.
*/
void GameWorld::setTile(int x, int y, TileType type)
{
    if (!isInsideGrid(x, y))
    {
        return;
    }

    if (tiles[y][x] == TileType::Body && type != TileType::Body && Body::isBodyAt(x, y))
    {
        Body::setTileUnderBodyAt(x, y, type);
        return;
    }

    tiles[y][x] = type;
}

/*
    Returns the tile type at a grid position.
    Outside-grid reads count as empty so callers have a safe fallback.
*/
TileType GameWorld::getTile(int x, int y) const
{
    if (!isInsideGrid(x, y))
    {
        return TileType::Empty;
    }

    return tiles[y][x];
}

/*
    Checks whether a grid coordinate is inside the world bounds.
*/
bool GameWorld::isInsideGrid(int x, int y) const
{
    return x >= 0 && x < gridWidth && y >= 0 && y < gridHeight;
}

/*
    Gives access to the SFML window for code that needs to draw directly.
*/
sf::RenderWindow& GameWorld::getWindow()
{
    return window;
}

/*
    Returns the number of tiles across the world.
*/
int GameWorld::getGridWidth() const
{
    return gridWidth;
}

/*
    Returns the number of tiles down the world.
*/
int GameWorld::getGridHeight() const
{
    return gridHeight;
}

/*
    Returns the pixel size of one grid cell.
*/
int GameWorld::getCellSize() const
{
    return cellSize;
}

/*
    Draws one tile using the color connected to its tile type.
*/
void GameWorld::drawTile(int x, int y, TileType type)
{
    if (!isInsideGrid(x, y))
    {
        return;
    }

    sf::RectangleShape square;
    square.setSize(sf::Vector2f(cellSize, cellSize));
    square.setPosition(x * cellSize, y * cellSize);
    square.setFillColor(getColorFromTile(type));

    window.draw(square);
}

/*
    Draws one tile using a custom color instead of a tile type.
*/
void GameWorld::drawTile(int x, int y, sf::Color color)
{
    if (!isInsideGrid(x, y))
    {
        return;
    }

    sf::RectangleShape square;
    square.setSize(sf::Vector2f(cellSize, cellSize));
    square.setPosition(x * cellSize, y * cellSize);
    square.setFillColor(DayNight::apply(color));

    window.draw(square);
}

/*
    Draws only the border around a tile.
    Useful for selection boxes, vision outlines, and debug highlights.
*/
void GameWorld::drawTileOutline(int x, int y, sf::Color color, float thickness)
{
    if (!isInsideGrid(x, y))
    {
        return;
    }

    sf::RectangleShape square;
    square.setSize(sf::Vector2f(cellSize, cellSize));
    square.setPosition(x * cellSize, y * cellSize);
    square.setFillColor(sf::Color::Transparent);
    square.setOutlineColor(color);
    square.setOutlineThickness(thickness);

    window.draw(square);
}

/*
    Draws a thick line between two pixel positions.
    SFML lines do not support thickness well, so this uses a rotated rectangle.
*/
void GameWorld::drawLine(float x1, float y1, float x2, float y2, sf::Color color, float thickness)
{
    sf::Vector2f direction(x2 - x1, y2 - y1);
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length == 0.0f)
    {
        return;
    }

    sf::RectangleShape line;
    line.setSize(sf::Vector2f(length, thickness));
    line.setFillColor(color);
    line.setPosition(x1, y1);

    // Converts the line direction into the angle needed to rotate the rectangle.
    float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159265f;
    line.setRotation(angle);

    window.draw(line);
}

/*
    Draws the grid lines over the whole world.
*/
void GameWorld::drawGrid()
{
    sf::VertexArray lines(sf::Lines);

    for (int x = 0; x <= gridWidth; ++x)
    {
        lines.append(sf::Vertex(sf::Vector2f(x * cellSize, 0), sf::Color::White));

        lines.append(sf::Vertex(sf::Vector2f(x * cellSize, windowHeight), sf::Color::White));
    }

    for (int y = 0; y <= gridHeight; ++y)
    {
        lines.append(sf::Vertex(sf::Vector2f(0, y * cellSize), sf::Color::White));

        lines.append(sf::Vertex(sf::Vector2f(windowWidth, y * cellSize), sf::Color::White));
    }

    window.draw(lines);
}

/*
    Converts a tile type into its current display color.
*/
sf::Color GameWorld::getColorFromTile(TileType type) const
{
    return TileColors::getDayNight(type);
}