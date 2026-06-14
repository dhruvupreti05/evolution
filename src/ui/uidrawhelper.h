#pragma once

#include <SFML/Graphics.hpp>
#include <string>

/*
    Shared drawing helpers for UI windows.
    This keeps repeated text and panel drawing code out of the inspector and stats classes.
*/
namespace UiDrawHelper
{
    void drawText(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded, const std::string& text, float x, float y, unsigned int size, sf::Color color = sf::Color::Black);

    void drawCenteredText(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded, const std::string& text, float centerX, float y, unsigned int size, sf::Color color = sf::Color::Black);

    void drawPanelBackground(sf::RenderWindow& window, float x, float y, float width, float height, sf::Color fillColor, sf::Color outlineColor = sf::Color::Black, float outlineThickness = 2.0f);
}