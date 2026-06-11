#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "core/gameworld.h"
#include "entities/player.h"

class PlayerInspector
{
public:
    PlayerInspector();

    void open();
    void close();

    bool isOpen() const;

    void handleEvents(GameWorld& world);
    void draw(GameWorld& world);

private:
    sf::RenderWindow window;
    sf::Font font;

    bool fontLoaded = false;
    int selectedPlayerIndex = 0;

    void moveToNextPlayer();
    void moveToPreviousPlayer();

    void moveSelectedPlayer(Direction direction, GameWorld& world);
    void updateInspectedPlayerId();

    void drawPanelBackground();
    void drawVision(const Player& player, const GameWorld& world);
    void drawInventory(const Player& player);
    void drawNeuralNetworkBox(float x, float y, float width, float height);
    void drawStats(const Player& player);

    void drawText(
        const std::string& text,
        float x,
        float y,
        unsigned int size = 24
    );

    void drawCenteredText(
        const std::string& text,
        float centerX,
        float y,
        unsigned int size = 24
    );

    sf::Color getColorFromTile(TileType tile) const;
};