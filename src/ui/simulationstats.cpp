#include "ui/simulationstats.h"
#include "core/config.h"
#include "environment/daynight.h"
#include "environment/weather.h"
#include "entities/human.h"
#include "entities/predator.h"
#include "environment/lake.h"
#include "entities/crop.h"
#include "core/paths.h"

#include "ui/uidrawhelper.h"

#include <algorithm>

/*
    Loads the font and weather icons used by the stats window.
*/
SimulationStats::SimulationStats()
{
    fontLoaded = font.loadFromFile("/System/Library/Fonts/Supplemental/Times New Roman.ttf");

    floodTextureLoaded = floodTexture.loadFromFile(Paths::getAssetPath("flood.png").string());

    droughtTextureLoaded = droughtTexture.loadFromFile(Paths::getAssetPath("drought.jpg").string());
}

/*
    Opens the stats window if it is not already open.
*/
void SimulationStats::open()
{
    if (window.isOpen())
    {
        return;
    }

    window.create(sf::VideoMode(Config::STATS_WINDOW_WIDTH, Config::STATS_WINDOW_HEIGHT), "Simulation Stats");
}

/*
    Closes the stats window.
*/
void SimulationStats::close()
{
    if (window.isOpen())
    {
        window.close();
    }
}

/*
    Returns whether the stats window is currently open.
*/
bool SimulationStats::isOpen() const
{
    return window.isOpen();
}

/*
    Handles stats-window events.
    Escape and the close button both close the window.
*/
void SimulationStats::handleEvents()
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
            if (event.key.code == sf::Keyboard::Escape)
            {
                close();
                return;
            }
        }
    }
}

/*
    Updates and draws the stats window.
*/
void SimulationStats::draw(GameWorld& world)
{
    if (!window.isOpen())
    {
        return;
    }

    handleEvents();

    if (!window.isOpen())
    {
        return;
    }

    window.clear(sf::Color(255, 244, 200));

    drawPanelBackground();

    drawCenteredText("Simulation Statistics", Config::STATS_WINDOW_WIDTH / 2.0f, 45.0f, 24);

    drawStatsText();
    drawWeatherBoxes();

    window.display();
}

/*
    Draws the main background panel for the stats window.
*/
void SimulationStats::drawPanelBackground()
{
    UiDrawHelper::drawPanelBackground(window, 20.0f, 20.0f, Config::STATS_WINDOW_WIDTH - 40.0f, Config::STATS_WINDOW_HEIGHT - 40.0f, sf::Color(255, 238, 200));
}

/*
    Draws the main simulation numbers.
*/
void SimulationStats::drawStatsText()
{
    float leftX = 55.0f;
    float rightX = 310.0f;

    float topY = 85.0f;
    float spacing = 38.0f;

    unsigned int size = 18;

    drawText("Humans Alive: " + std::to_string(Human::countAlive()), leftX, topY, size);

    drawText("Humans Dead: " + std::to_string(Human::countDead()), leftX, topY + spacing, size);

    drawText("Predators Alive: " + std::to_string(Predator::countAlive()), leftX, topY + spacing * 2.0f, size);

    drawText("Predators Dead: " + std::to_string(Predator::countDead()), leftX, topY + spacing * 3.0f, size);

    std::string dayNight = "Day";

    if (DayNight::getDarknessAmount() > Config::MIDNIGHT_RGB_DECREASE / 2)
    {
        dayNight = "Night";
    }

    drawText("Day / Night: " + dayNight, rightX, topY, size);

    drawText("Water Blocks: " + std::to_string(Lake::getTotalWaterBlocks()), rightX, topY + spacing, size);

    drawText("Crop Blocks: " + std::to_string(Crop::getCount()), rightX, topY + spacing * 2.0f, size);
}

/*
    Draws the flood and drought alert boxes.
*/
void SimulationStats::drawWeatherBoxes()
{
    float boxWidth = 110.0f;
    float boxHeight = 85.0f;

    float leftX = 135.0f;
    float rightX = 315.0f;
    float y = 280.0f;

    drawWeatherBox(leftX, y, boxWidth, boxHeight, "Flood", Weather::isFloodAlertActive());

    drawWeatherBox(rightX, y, boxWidth, boxHeight, "Drought", Weather::isDroughtAlertActive());
}

/*
    Draws one weather alert box.
    If the icon failed to load, it falls back to text inside the box.
*/
void SimulationStats::drawWeatherBox(float x, float y, float width, float height, const std::string& title, bool active)
{
    sf::RectangleShape box;
    box.setSize(sf::Vector2f(width, height));
    box.setPosition(x, y);

    if (active)
    {
        if (title == "Flood")
        {
            box.setFillColor(sf::Color(80, 150, 255));
        }
        else if (title == "Drought")
        {
            box.setFillColor(sf::Color(255, 220, 120));
        }
        else
        {
            box.setFillColor(sf::Color(255, 220, 120));
        }
    }
    else
    {
        box.setFillColor(sf::Color::White);
    }

    box.setOutlineThickness(0);

    window.draw(box);

    sf::Sprite icon;

    if (title == "Flood" && floodTextureLoaded)
    {
        icon.setTexture(floodTexture);
    }
    else if (title == "Drought" && droughtTextureLoaded)
    {
        icon.setTexture(droughtTexture);
    }
    else
    {
        drawCenteredText(title, x + width / 2.0f, y + height / 2.0f, 16);
        drawCenteredText(title, x + width / 2.0f, y + height + 24.0f, 20);
        return;
    }

    sf::FloatRect bounds = icon.getLocalBounds();

    float scaleX = (width - 18.0f) / bounds.width;
    float scaleY = (height - 18.0f) / bounds.height;
    float scale = std::min(scaleX, scaleY);

    icon.setScale(scale, scale);

    sf::FloatRect scaledBounds = icon.getGlobalBounds();

    // Centers the scaled icon inside the weather box.
    icon.setPosition(x + (width - scaledBounds.width) / 2.0f, y + (height - scaledBounds.height) / 2.0f);

    window.draw(icon);

    drawCenteredText(title, x + width / 2.0f, y + height + 24.0f, 20);
}

/*
    Draws left-aligned text using the shared UI text helper.
*/
void SimulationStats::drawText(const std::string& text, float x, float y, unsigned int size)
{
    UiDrawHelper::drawText(window, font, fontLoaded, text, x, y, size);
}

/*
    Draws centered text using the shared UI text helper.
*/
void SimulationStats::drawCenteredText(const std::string& text, float centerX, float y, unsigned int size)
{
    UiDrawHelper::drawCenteredText(window, font, fontLoaded, text, centerX, y, size);
}