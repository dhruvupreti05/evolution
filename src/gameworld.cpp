#include "gameworld.h"

GameWorld::GameWorld(
    int windowWidth,
    int windowHeight,
    int gridSize,
    int cellSize,
    const std::string& title
)
    : window(sf::VideoMode(windowWidth, windowHeight), title),
      windowWidth(windowWidth),
      windowHeight(windowHeight),
      gridSize(gridSize),
      cellSize(cellSize),
      tiles(gridSize, std::vector<TileType>(gridSize, TileType::Empty))
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
    window.clear(sf::Color(235, 220, 180));
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
    return x >= 0 && x < gridSize && y >= 0 && y < gridSize;
}

sf::RenderWindow& GameWorld::getWindow()
{
    return window;
}

int GameWorld::getGridSize() const
{
    return gridSize;
}

int GameWorld::getCellSize() const
{
    return cellSize;
}

sf::Color GameWorld::getColorFromTile(TileType type) const
{
    switch (type)
    {
        case TileType::Water:
            return sf::Color::Blue;

        case TileType::Sand:
            return sf::Color(194, 178, 128);

        case TileType::Tree:
            return sf::Color(34, 139, 34);

        case TileType::Food:
            return sf::Color::Red;

        case TileType::Human:
            return sf::Color::White;

        case TileType::Empty:
        default:
            return sf::Color(235, 220, 180);
    }
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

void GameWorld::drawGrid()
{
    sf::VertexArray lines(sf::Lines);

    for (int i = 0; i <= gridSize; ++i)
    {
        lines.append(
            sf::Vertex(
                sf::Vector2f(i * cellSize, 0),
                sf::Color::White
            )
        );

        lines.append(
            sf::Vertex(
                sf::Vector2f(i * cellSize, windowHeight),
                sf::Color::White
            )
        );

        lines.append(
            sf::Vertex(
                sf::Vector2f(0, i * cellSize),
                sf::Color::White
            )
        );

        lines.append(
            sf::Vertex(
                sf::Vector2f(windowWidth, i * cellSize),
                sf::Color::White
            )
        );
    }

    window.draw(lines);
}