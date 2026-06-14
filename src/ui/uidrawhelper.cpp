#include "ui/uidrawhelper.h"

/*
    Draws normal left-aligned text.
    If the font failed to load, it skips drawing instead of crashing or showing broken text.
*/
void UiDrawHelper::drawText(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded, const std::string& text, float x, float y, unsigned int size, sf::Color color)
{
    if (!fontLoaded)
    {
        return;
    }

    sf::Text label;
    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(size);
    label.setFillColor(color);
    label.setPosition(x, y);

    window.draw(label);
}

/*
    Draws text centered around a given x position.
    The origin is shifted to the middle of the text bounds before positioning.
*/
void UiDrawHelper::drawCenteredText(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded, const std::string& text, float centerX, float y, unsigned int size, sf::Color color)
{
    if (!fontLoaded)
    {
        return;
    }

    sf::Text label;
    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(size);
    label.setFillColor(color);

    sf::FloatRect bounds = label.getLocalBounds();

    // SFML text bounds can have an offset, so bounds.left/top are included when centering.
    label.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);

    label.setPosition(centerX, y);

    window.draw(label);
}

/*
    Draws a reusable rectangle panel for UI windows.
*/
void UiDrawHelper::drawPanelBackground(sf::RenderWindow& window, float x, float y, float width, float height, sf::Color fillColor, sf::Color outlineColor, float outlineThickness)
{
    sf::RectangleShape panel;
    panel.setSize(sf::Vector2f(width, height));
    panel.setPosition(x, y);
    panel.setFillColor(fillColor);
    panel.setOutlineColor(outlineColor);
    panel.setOutlineThickness(outlineThickness);

    window.draw(panel);
}