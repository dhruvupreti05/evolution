#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "core/gameworld.h"
#include "entities/human.h"
#include "entities/action.h"

/*
    Separate UI window for inspecting and manually controlling one human.
    It shows stats, vision, inventory, and lets the user send manual actions.
*/
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

    // Index into the shared human list for the human currently being inspected.
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

    void drawText(const std::string& text, float x, float y, unsigned int size = 24);

    void drawCenteredText(const std::string& text, float centerX, float y, unsigned int size = 24);

    sf::Color getColorFromTile(TileType tile) const;
};