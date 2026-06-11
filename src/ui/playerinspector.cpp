#include "playerinspector.h"
#include "core/config.h"

#include <vector>
#include <string>
#include <iostream>

PlayerInspector::PlayerInspector()
{
    fontLoaded = font.loadFromFile(
        "/System/Library/Fonts/Supplemental/Times New Roman.ttf"
    );
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

        if (event.type != sf::Event::KeyPressed)
        {
            continue;
        }

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

        if (event.key.code == sf::Keyboard::E)
        {
            std::vector<Player>& players = Player::getPlayers();

            if (!players.empty())
            {
                if (
                    selectedPlayerIndex < 0 ||
                    selectedPlayerIndex >= static_cast<int>(players.size())
                )
                {
                    selectedPlayerIndex = 0;
                }

                players[selectedPlayerIndex].tryPickUp(world);
            }
        }

        if (event.key.code == sf::Keyboard::R)
        {
            std::vector<Player>& players = Player::getPlayers();

            if (!players.empty())
            {
                if (
                    selectedPlayerIndex < 0 ||
                    selectedPlayerIndex >= static_cast<int>(players.size())
                )
                {
                    selectedPlayerIndex = 0;
                }

                players[selectedPlayerIndex].tryDrop(world);
            }
        }

        if (event.key.code == sf::Keyboard::Escape)
        {
            close();
            return;
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

    if (
        selectedPlayerIndex < 0 ||
        selectedPlayerIndex >= static_cast<int>(players.size())
    )
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

        drawCenteredText(
            "No players exist.",
            Config::INSPECTOR_WINDOW_WIDTH / 2.0f,
            60.0f,
            24
        );

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
    drawInventory(player);

    float centerX = Config::INSPECTOR_WINDOW_WIDTH / 2.0f;

    float neuralWidth = Config::INSPECTOR_WINDOW_WIDTH - 110.0f;
    float neuralHeight = 250.0f;
    float neuralX = centerX - neuralWidth / 2.0f;
    float neuralY = 345.0f;

    drawNeuralNetworkBox(neuralX, neuralY, neuralWidth, neuralHeight);
    drawStats(player);

    window.display();
}

void PlayerInspector::drawInventory(const Player& player)
{
    float centerX = Config::INSPECTOR_WINDOW_WIDTH / 2.0f;

    float startY = 290.0f;
    float tileSize = 24.0f;
    float spacing = 5.0f;
    float title_inventory_space = 15.0f;

    int slotCount = Config::HUMAN_INVENTORY_SIZE;

    float totalWidth =
        slotCount * tileSize +
        (slotCount - 1) * spacing;

    float startX = centerX - totalWidth / 2.0f;

    drawCenteredText("Inventory", centerX, startY, 18);

    const std::vector<TileType>& inventory = player.getInventory();

    for (int i = 0; i < slotCount; ++i)
    {
        float slotX = startX + i * (tileSize + spacing);
        float slotY = startY + title_inventory_space;

        sf::RectangleShape slot;
        slot.setSize(sf::Vector2f(tileSize, tileSize));
        slot.setPosition(slotX, slotY);
        slot.setFillColor(Config::COLOR_BACKGROUND);
        slot.setOutlineColor(sf::Color::Black);
        slot.setOutlineThickness(1);

        window.draw(slot);

        if (i < static_cast<int>(inventory.size()))
        {
            sf::RectangleShape item;
            item.setSize(sf::Vector2f(tileSize - 6.0f, tileSize - 6.0f));
            item.setPosition(slotX + 3.0f, slotY + 3.0f);
            item.setFillColor(getColorFromTile(inventory[i]));

            window.draw(item);
        }
    }
}

void PlayerInspector::drawPanelBackground()
{
    sf::RectangleShape panel;

    panel.setSize(
        sf::Vector2f(
            Config::INSPECTOR_WINDOW_WIDTH - 40.0f,
            Config::INSPECTOR_WINDOW_HEIGHT - 40.0f
        )
    );

    panel.setPosition(20.0f, 20.0f);
    panel.setFillColor(sf::Color(255, 238, 200));
    panel.setOutlineColor(sf::Color::Black);
    panel.setOutlineThickness(2);

    window.draw(panel);
}

void PlayerInspector::drawVision(const Player& player, const GameWorld& world)
{
    int tileSize = Config::INSPECTOR_VIEW_TILE_SIZE;

    float centerX = Config::INSPECTOR_WINDOW_WIDTH / 2.0f;

    int playerDrawX = static_cast<int>(centerX - tileSize / 2.0f);
    int playerDrawY = 250;

    drawCenteredText("Player View", centerX, 55.0f, 22);

    std::vector<VisibleTile> visibleTiles = player.getVisibleTiles(world);

    for (int forward = 1; forward <= Config::HUMAN_VISION_RANGE; ++forward)
    {
        int sideLimit = forward - 1;

        for (int side = -sideLimit; side <= sideLimit; ++side)
        {
            int drawX = playerDrawX + side * tileSize;
            int drawY = playerDrawY - forward * tileSize;

            sf::RectangleShape square;
            square.setSize(sf::Vector2f(tileSize - 2.0f, tileSize - 2.0f));
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
        square.setSize(sf::Vector2f(tileSize - 2.0f, tileSize - 2.0f));
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
}

void PlayerInspector::drawStats(const Player& player)
{
    float centerX = Config::INSPECTOR_WINDOW_WIDTH / 2.0f;

    float leftX = centerX - 150.0f;
    float rightX = centerX + 10.0f;

    float y = 610.0f;
    float spacing = 26.0f;

    unsigned int statTextSize = 18;

    drawText(
        "Player ID: " + std::to_string(player.getId()),
        leftX,
        y,
        statTextSize
    );

    drawText(
        "Age: " + std::to_string(player.getAge()),
        leftX,
        y + spacing,
        statTextSize
    );

    drawText(
        "Health: " + std::to_string(player.getHealth()),
        leftX,
        y + spacing * 2.0f,
        statTextSize
    );

    drawText(
        "Thirst: " + std::to_string(player.getThirst()),
        leftX,
        y + spacing * 3.0f,
        statTextSize
    );

    drawText(
        "Hunger: " + std::to_string(player.getHunger()),
        rightX,
        y,
        statTextSize
    );

    drawText(
        "Gender: " + player.getGenderString(),
        rightX,
        y + spacing,
        statTextSize
    );

    drawText(
        "Orientation: " + player.getOrientationString(),
        rightX,
        y + spacing * 2.0f,
        statTextSize
    );

    std::string status = player.isDead() ? "Dead" : "Alive";

    drawText(
        "Status: " + status,
        rightX,
        y + spacing * 3.0f,
        statTextSize
    );
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

    drawText("Neural Network", x + 12.0f, y + 10.0f, 16);
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

void PlayerInspector::drawCenteredText(
    const std::string& text,
    float centerX,
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

    sf::FloatRect bounds = label.getLocalBounds();

    label.setOrigin(
        bounds.left + bounds.width / 2.0f,
        bounds.top + bounds.height / 2.0f
    );

    label.setPosition(centerX, y);

    window.draw(label);
}

sf::Color PlayerInspector::getColorFromTile(TileType tile) const
{
    sf::Color color;

    switch (tile)
    {
        case TileType::Water:
            color = Config::COLOR_WATER;
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

        case TileType::Sand:
            color = Config::COLOR_SAND;
            break;

        case TileType::Empty:
        default:
            color = Config::COLOR_BACKGROUND;
            break;
    }

    return DayNight::apply(color);
}