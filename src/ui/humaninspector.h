#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "core/gameworld.h"
#include "entities/human.h"
#include "entities/action.h"

class HumanInspector
{
public:
    HumanInspector();

    void open();
    void close();

    bool isOpen() const;

    void handleEvents(GameWorld& world);
    void draw(GameWorld& world);

    void mateFacingEntity();

private:
    bool fontLoaded = false;

    sf::RenderWindow window;
    sf::Font font;

    int selectedHumanIndex = 0;

    void moveToNextHuman();
    void moveToPreviousHuman();

    void moveSelectedHuman(Direction direction);
    void updateInspectedHumanId();

    void resetAllHumanBrains();

    Human* getSelectedHuman();

    void eatFacingTile();
    void drinkFacingTile();
    void attackFacingTile();

    void pickUpItem();
    void dropItem();

    void drawPanelBackground();
    void drawVision(const Human& human, const GameWorld& world);
    void drawInventory(const Human& human);
    void drawNeuralNetworkBox(float x, float y, float width, float height);
    void drawStats(const Human& human);

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