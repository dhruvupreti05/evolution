#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "core/gameworld.h"

class SimulationStats
{
public:
    SimulationStats();

    void open();
    void close();

    bool isOpen() const;

    void handleEvents();
    void draw(GameWorld& world);

private:
    sf::Texture floodTexture;
    sf::Texture droughtTexture;

    bool floodTextureLoaded = false;
    bool droughtTextureLoaded = false;
    
    sf::RenderWindow window;
    sf::Font font;

    bool fontLoaded = false;

    void drawPanelBackground();
    void drawStatsText();
    void drawWeatherBoxes();

    void drawText(
        const std::string& text,
        float x,
        float y,
        unsigned int size = 28
    );

    void drawCenteredText(
        const std::string& text,
        float centerX,
        float y,
        unsigned int size = 28
    );

    void drawWeatherBox(
        float x,
        float y,
        float width,
        float height,
        const std::string& title,
        bool active
    );
};