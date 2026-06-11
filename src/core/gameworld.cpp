#include "core/gameworld.h"
#include "core/daynight.h"
#include "core/config.h"

#include <cmath>

GameWorld::GameWorld(
    int windowWidth,
    int windowHeight,
    int gridWidth,
    int gridHeight,
    int cellSize,
    const std::string& title
)
    : window(sf::VideoMode(windowWidth, windowHeight), title),
      windowWidth(windowWidth),
      windowHeight(windowHeight),
      gridWidth(gridWidth),
      gridHeight(gridHeight),
      cellSize(cellSize),
      tiles(gridHeight, std::vector<TileType>(gridWidth, TileType::Empty))
{
}

bool GameWorld::isOpen() const
{
    return window.isOpen();
}

bool GameWorld::pollEvent(sf::Event& event)
{
    return window.pollEvent(event);
}

void GameWorld::clear()
{
    window.clear(DayNight::apply(Config::COLOR_BACKGROUND));
}

void GameWorld::display()
{
    window.display();
}

void GameWorld::setTile(int x, int y, TileType type)
{
    if (!isInsideGrid(x, y))
    {
        return;
    }

    tiles[y][x] = type;
}

TileType GameWorld::getTile(int x, int y) const
{
    if (!isInsideGrid(x, y))
    {
        return TileType::Empty;
    }

    return tiles[y][x];
}

bool GameWorld::isInsideGrid(int x, int y) const
{
    return x >= 0 && x < gridWidth && y >= 0 && y < gridHeight;
}

sf::RenderWindow& GameWorld::getWindow()
{
    return window;
}

int GameWorld::getGridWidth() const
{
    return gridWidth;
}

int GameWorld::getGridHeight() const
{
    return gridHeight;
}

int GameWorld::getCellSize() const
{
    return cellSize;
}

sf::Color GameWorld::getColorFromTile(TileType type) const
{
    sf::Color color;

    switch (type)
    {
        case TileType::Water:
            color = Config::COLOR_WATER;
            break;

        case TileType::Sand:
            color = Config::COLOR_SAND;
            break;

        case TileType::Tree:
            color = Config::COLOR_TREE;
            break;

        case TileType::Food:
            color = Config::COLOR_FOOD;
            break;

        case TileType::Human:
            color = Config::COLOR_HUMAN;
            break;

        case TileType::Predator:
            color = Config::COLOR_PREDATOR;
            break;

        case TileType::Empty:
        default:
            color = Config::COLOR_BACKGROUND;
            break;
    }

    return DayNight::apply(color);
}

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

void GameWorld::drawLine(
    float x1,
    float y1,
    float x2,
    float y2,
    sf::Color color,
    float thickness
)
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

    float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159265f;
    line.setRotation(angle);

    window.draw(line);
}

void GameWorld::drawGrid()
{
    sf::VertexArray lines(sf::Lines);

    for (int x = 0; x <= gridWidth; ++x)
    {
        lines.append(
            sf::Vertex(
                sf::Vector2f(x * cellSize, 0),
                sf::Color::White
            )
        );

        lines.append(
            sf::Vertex(
                sf::Vector2f(x * cellSize, windowHeight),
                sf::Color::White
            )
        );
    }

    for (int y = 0; y <= gridHeight; ++y)
    {
        lines.append(
            sf::Vertex(
                sf::Vector2f(0, y * cellSize),
                sf::Color::White
            )
        );

        lines.append(
            sf::Vertex(
                sf::Vector2f(windowWidth, y * cellSize),
                sf::Color::White
            )
        );
    }

    window.draw(lines);
}