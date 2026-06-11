#include "playerinspector.h"
#include "config.h"

#include <vector>
#include <string>
#include <iostream>

PlayerInspector::PlayerInspector()
{
    fontLoaded = font.loadFromFile("assets/fonts/arial.ttf");

    if (!fontLoaded)
    {
        fontLoaded = font.loadFromFile(
            "/System/Library/Fonts/Supplemental/Arial.ttf"
        );
    }

    if (!fontLoaded)
    {
        std::cerr << "Could not load inspector font.\n";
    }
}

void PlayerInspector::open()
{
    if (window.isOpen())
    {
        return;
    }

    window.create(
        sf::VideoMode(
            Config::INSPECTOR_WINDOW_WIDTH,
            Config::INSPECTOR_WINDOW_HEIGHT
        ),
        "Player Inspector"
    );
}

void PlayerInspector::close()
{
    if (window.isOpen())
    {
        window.close();
    }

    Player::clearInspectedPlayer();
}

bool PlayerInspector::isOpen() const
{
    return window.isOpen();
}

void PlayerInspector::handleEvents()
{
    if (!window.isOpen())
    {
        return;
    }

    sf::Event event;

    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            close();
            return;
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Right)
            {
                moveToNextPlayer();
            }

            if (event.key.code == sf::Keyboard::Left)
            {
                moveToPreviousPlayer();
            }

            if (event.key.code == sf::Keyboard::Escape)
            {
                close();
                return;
            }
        }
    }
}

void PlayerInspector::moveToNextPlayer()
{
    std::vector<Player>& players = Player::getPlayers();

    if (players.empty())
    {
        return;
    }

    selectedPlayerIndex++;

    if (selectedPlayerIndex >= static_cast<int>(players.size()))
    {
        selectedPlayerIndex = 0;
    }
}

void PlayerInspector::moveToPreviousPlayer()
{
    std::vector<Player>& players = Player::getPlayers();

    if (players.empty())
    {
        return;
    }

    selectedPlayerIndex--;

    if (selectedPlayerIndex < 0)
    {
        selectedPlayerIndex = static_cast<int>(players.size()) - 1;
    }
}

void PlayerInspector::draw(GameWorld& world)
{
    if (!window.isOpen())
    {
        Player::clearInspectedPlayer();
        return;
    }

    handleEvents();

    if (!window.isOpen())
    {
        Player::clearInspectedPlayer();
        return;
    }

    window.clear(sf::Color(255, 244, 200));

    drawPanelBackground();

    std::vector<Player>& players = Player::getPlayers();

    if (players.empty())
    {
        Player::clearInspectedPlayer();

        drawText("No players exist.", 40, 40, 24);
        window.display();
        return;
    }

    if (selectedPlayerIndex >= static_cast<int>(players.size()))
    {
        selectedPlayerIndex = 0;
    }

    const Player& player = players[selectedPlayerIndex];

    Player::setInspectedPlayerId(player.getId());

    drawVision(player, world);
    drawStats(player);

    window.display();
}

void PlayerInspector::drawPanelBackground()
{
    sf::RectangleShape panel;

    panel.setSize(
        sf::Vector2f(
            Config::INSPECTOR_WINDOW_WIDTH - 40,
            Config::INSPECTOR_WINDOW_HEIGHT - 40
        )
    );

    panel.setPosition(20, 20);
    panel.setFillColor(sf::Color(255, 238, 200));
    panel.setOutlineColor(sf::Color::Black);
    panel.setOutlineThickness(2);

    window.draw(panel);
}

void PlayerInspector::drawVision(const Player& player, const GameWorld& world)
{
    int tileSize = Config::INSPECTOR_VIEW_TILE_SIZE;

    int centerX = Config::INSPECTOR_WINDOW_WIDTH / 2;
    int playerDrawX = centerX - tileSize / 2;
    int playerDrawY = 250;

    std::vector<VisibleTile> visibleTiles = player.getVisibleTiles(world);

    for (int forward = 1; forward <= Config::HUMAN_VISION_RANGE; ++forward)
    {
        int sideLimit = forward - 1;

        for (int side = -sideLimit; side <= sideLimit; ++side)
        {
            int drawX = playerDrawX + side * tileSize;
            int drawY = playerDrawY - forward * tileSize;

            sf::RectangleShape square;
            square.setSize(sf::Vector2f(tileSize - 2, tileSize - 2));
            square.setPosition(drawX, drawY);
            square.setFillColor(sf::Color(245, 245, 245));
            square.setOutlineColor(sf::Color(215, 215, 215));
            square.setOutlineThickness(1);

            window.draw(square);
        }
    }

    for (const auto& tile : visibleTiles)
    {
        int drawX = playerDrawX + tile.side * tileSize;
        int drawY = playerDrawY - tile.forward * tileSize;

        sf::RectangleShape square;
        square.setSize(sf::Vector2f(tileSize - 2, tileSize - 2));
        square.setPosition(drawX, drawY);
        square.setFillColor(getColorFromTile(tile.tile));
        square.setOutlineColor(sf::Color::Black);
        square.setOutlineThickness(1);

        window.draw(square);
    }

    sf::RectangleShape playerSquare;
    playerSquare.setSize(sf::Vector2f(tileSize, tileSize));
    playerSquare.setPosition(playerDrawX, playerDrawY);
    playerSquare.setFillColor(sf::Color::White);
    playerSquare.setOutlineColor(sf::Color::Black);
    playerSquare.setOutlineThickness(2);

    window.draw(playerSquare);

    drawText("Player View", 145, 38, 22);
}

void PlayerInspector::drawStats(const Player& player)
{
    float x = 55;
    float y = Config::INSPECTOR_STATS_START_Y;
    float spacing = 44;

    drawText("Player ID: " + std::to_string(player.getId()), x, y);
    y += spacing;

    drawText("Age: " + std::to_string(player.getAge()), x, y);
    y += spacing;

    drawText("Health: " + std::to_string(player.getHealth()), x, y);
    y += spacing;

    drawText("Thirst: " + std::to_string(player.getThirst()), x, y);
    y += spacing;

    drawText("Hunger: " + std::to_string(player.getHunger()), x, y);
    y += spacing;

    drawText("Gender: " + player.getGenderString(), x, y);
    y += spacing;

    drawText("Orientation: " + player.getOrientationString(), x, y);
    y += spacing;

    std::string status = player.isDead() ? "Dead" : "Alive";
    drawText("Status: " + status, x, y);
}

void PlayerInspector::drawText(
    const std::string& text,
    float x,
    float y,
    unsigned int size
)
{
    if (!fontLoaded)
    {
        return;
    }

    sf::Text label;
    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(size);
    label.setFillColor(sf::Color::Black);
    label.setPosition(x, y);

    window.draw(label);
}

sf::Color PlayerInspector::getColorFromTile(TileType tile) const
{
    switch (tile)
    {
        case TileType::Water:
            return sf::Color::Blue;

        case TileType::Tree:
            return sf::Color(34, 139, 34);

        case TileType::Food:
            return sf::Color::Red;

        case TileType::Human:
            return sf::Color::White;

        case TileType::Sand:
            return sf::Color(194, 178, 128);

        case TileType::Empty:
        default:
            return sf::Color(245, 245, 245);
    }
}