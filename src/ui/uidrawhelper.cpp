#include "ui/uidrawhelper.h"

void UiDrawHelper::drawText(
    sf::RenderWindow& window,
    const sf::Font& font,
    bool fontLoaded,
    const std::string& text,
    float x,
    float y,
    unsigned int size,
    sf::Color color
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
    label.setFillColor(color);
    label.setPosition(x, y);

    window.draw(label);
}

void UiDrawHelper::drawCenteredText(
    sf::RenderWindow& window,
    const sf::Font& font,
    bool fontLoaded,
    const std::string& text,
    float centerX,
    float y,
    unsigned int size,
    sf::Color color
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
    label.setFillColor(color);

    sf::FloatRect bounds = label.getLocalBounds();

    label.setOrigin(
        bounds.left + bounds.width / 2.0f,
        bounds.top + bounds.height / 2.0f
    );

    label.setPosition(centerX, y);

    window.draw(label);
}

void UiDrawHelper::drawPanelBackground(
    sf::RenderWindow& window,
    float x,
    float y,
    float width,
    float height,
    sf::Color fillColor,
    sf::Color outlineColor,
    float outlineThickness
)
{
    sf::RectangleShape panel;
    panel.setSize(sf::Vector2f(width, height));
    panel.setPosition(x, y);
    panel.setFillColor(fillColor);
    panel.setOutlineColor(outlineColor);
    panel.setOutlineThickness(outlineThickness);

    window.draw(panel);
}