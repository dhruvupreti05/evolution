#include "playerinspector.h"
#include "config.h"

#include <vector>
#include <string>
#include <iostream>

PlayerInspector::PlayerInspector()
{
    fontLoaded = font.loadFromFile("/System/Library/Fonts/Supplemental/Times New Roman.ttf");
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

    updateInspectedPlayerId();
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

void PlayerInspector::handleEvents(GameWorld& world)
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

            if (event.key.code == sf::Keyboard::W)
            {
                moveSelectedPlayer(Direction::Up, world);
            }

            if (event.key.code == sf::Keyboard::A)
            {
                moveSelectedPlayer(Direction::Left, world);
            }

            if (event.key.code == sf::Keyboard::S)
            {
                moveSelectedPlayer(Direction::Down, world);
            }

            if (event.key.code == sf::Keyboard::D)
            {
                moveSelectedPlayer(Direction::Right, world);
            }

            if (event.key.code == sf::Keyboard::Escape)
            {
                close();
                return;
            }
        }
    }
}

void PlayerInspector::moveSelectedPlayer(Direction direction, GameWorld& world)
{
    std::vector<Player>& players = Player::getPlayers();

    if (players.empty())
    {
        return;
    }

    if (selectedPlayerIndex < 0 || selectedPlayerIndex >= static_cast<int>(players.size()))
    {
        selectedPlayerIndex = 0;
    }

    Player& player = players[selectedPlayerIndex];

    if (player.isDead())
    {
        return;
    }

    player.controlledMove(direction, world);
}

void PlayerInspector::updateInspectedPlayerId()
{
    std::vector<Player>& players = Player::getPlayers();

    if (players.empty())
    {
        Player::clearInspectedPlayer();
        return;
    }

    if (selectedPlayerIndex < 0)
    {
        selectedPlayerIndex = 0;
    }

    if (selectedPlayerIndex >= static_cast<int>(players.size()))
    {
        selectedPlayerIndex = 0;
    }

    Player::setInspectedPlayerId(players[selectedPlayerIndex].getId());
}

void PlayerInspector::moveToNextPlayer()
{
    std::vector<Player>& players = Player::getPlayers();

    if (players.empty())
    {
        Player::clearInspectedPlayer();
        return;
    }

    selectedPlayerIndex++;

    if (selectedPlayerIndex >= static_cast<int>(players.size()))
    {
        selectedPlayerIndex = 0;
    }

    updateInspectedPlayerId();
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

    updateInspectedPlayerId();
}

void PlayerInspector::draw(GameWorld& world)
{
    if (!window.isOpen())
    {
        Player::clearInspectedPlayer();
        return;
    }

    handleEvents(world);

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
    drawNeuralNetworkBox(55, 300, Config::INSPECTOR_WINDOW_WIDTH - 110, 230);
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
            square.setFillColor(Config::COLOR_BACKGROUND);
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
    float leftX = 70;
    float rightX = 230;
    float y = 610;
    float spacing = 26;

    unsigned int statTextSize = 18;

    // Left column
    drawText("Player ID: " + std::to_string(player.getId()), leftX, y, statTextSize);
    drawText("Age: " + std::to_string(player.getAge()), leftX, y + spacing, statTextSize);
    drawText("Health: " + std::to_string(player.getHealth()), leftX, y + spacing * 2, statTextSize);
    drawText("Thirst: " + std::to_string(player.getThirst()), leftX, y + spacing * 3, statTextSize);

    // Right column
    drawText("Hunger: " + std::to_string(player.getHunger()), rightX, y, statTextSize);
    drawText("Gender: " + player.getGenderString(), rightX, y + spacing, statTextSize);
    drawText("Orientation: " + player.getOrientationString(), rightX, y + spacing * 2, statTextSize);

    std::string status = player.isDead() ? "Dead" : "Alive";
    drawText("Status: " + status, rightX, y + spacing * 3, statTextSize);
}

void PlayerInspector::drawNeuralNetworkBox(
    float x,
    float y,
    float width,
    float height
)
{
    sf::RectangleShape box;
    box.setSize(sf::Vector2f(width, height));
    box.setPosition(x, y);
    box.setFillColor(Config::COLOR_BACKGROUND);
    box.setOutlineColor(sf::Color::Black);
    box.setOutlineThickness(2);

    window.draw(box);

    drawText("Neural Network", x + 12, y + 10, 16);
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
            return Config::COLOR_WATER;

        case TileType::Tree:
            return Config::COLOR_TREE;

        case TileType::Food:
            return Config::COLOR_FOOD;

        case TileType::Human:
            return Config::COLOR_HUMAN;

        case TileType::Sand:
            return Config::COLOR_SAND;

        case TileType::Empty:
        default:
            return Config::COLOR_BACKGROUND;
    }
}