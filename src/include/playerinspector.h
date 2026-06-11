#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "gameworld.h"
#include "player.h"

class PlayerInspector
{
public:
    PlayerInspector();

    void open();
    void close();

    bool isOpen() const;

    void handleEvents();
    void draw(GameWorld& world);

private:
    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded = false;

    int selectedPlayerIndex = 0;

    void moveToNextPlayer();
    void moveToPreviousPlayer();

    void drawPanelBackground();
    void drawVision(const Player& player, const GameWorld& world);
    void drawStats(const Player& player);

    void drawText(
        const std::string& text,
        float x,
        float y,
        unsigned int size = 24
    );

    sf::Color getColorFromTile(TileType tile) const;
};