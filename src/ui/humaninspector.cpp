#include "humaninspector.h"
#include "core/config.h"
#include "entities/action.h"

#include <vector>
#include <string>
#include <iostream>

namespace
{
    void getFacingTarget(const Human& human, int& targetX, int& targetY)
    {
        targetX = human.getX();
        targetY = human.getY();

        switch (human.getOrientation())
        {
            case Orientation::North:
                targetY--;
                break;

            case Orientation::South:
                targetY++;
                break;

            case Orientation::East:
                targetX++;
                break;

            case Orientation::West:
                targetX--;
                break;
        }
    }
}


HumanInspector::HumanInspector()
{
    fontLoaded = font.loadFromFile(
        "/System/Library/Fonts/Supplemental/Times New Roman.ttf"
    );
}

void HumanInspector::open()
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
        "Human Inspector"
    );

    updateInspectedHumanId();
}

void HumanInspector::close()
{
    if (window.isOpen())
    {
        window.close();
    }

    std::vector<Human>& humans = Human::getHumans();

    if (
        selectedHumanIndex >= 0 &&
        selectedHumanIndex < static_cast<int>(humans.size())
    )
    {
        humans[selectedHumanIndex].setRandomBrain();
    }

    Human::clearInspectedHuman();
}

bool HumanInspector::isOpen() const
{
    return window.isOpen();
}

void HumanInspector::handleEvents(GameWorld& world)
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
            moveToNextHuman();
        }

        if (event.key.code == sf::Keyboard::Left)
        {
            moveToPreviousHuman();
        }

        if (event.key.code == sf::Keyboard::W)
        {
            moveSelectedHuman(Direction::Up, world);
        }

        if (event.key.code == sf::Keyboard::A)
        {
            moveSelectedHuman(Direction::Left, world);
        }

        if (event.key.code == sf::Keyboard::S)
        {
            moveSelectedHuman(Direction::Down, world);
        }

        if (event.key.code == sf::Keyboard::D)
        {
            moveSelectedHuman(Direction::Right, world);
        }

        if (event.key.code == sf::Keyboard::E)
        {
            std::vector<Human>& humans = Human::getHumans();

            if (!humans.empty())
            {
                if (
                    selectedHumanIndex < 0 ||
                    selectedHumanIndex >= static_cast<int>(humans.size())
                )
                {
                    selectedHumanIndex = 0;
                }

                int targetX;
                int targetY;
                getFacingTarget(humans[selectedHumanIndex], targetX, targetY);
                humans[selectedHumanIndex].giveManualAction(Action::eat(targetX, targetY));
            }
        }

        if (event.key.code == sf::Keyboard::R)
        {
            std::vector<Human>& humans = Human::getHumans();

            if (!humans.empty())
            {
                if (
                    selectedHumanIndex < 0 ||
                    selectedHumanIndex >= static_cast<int>(humans.size())
                )
                {
                    selectedHumanIndex = 0;
                }

                int targetX;
                int targetY;
                getFacingTarget(humans[selectedHumanIndex], targetX, targetY);
                humans[selectedHumanIndex].giveManualAction(Action::drink(targetX, targetY));
            }
        }

        if (event.key.code == sf::Keyboard::F)
        {
            std::vector<Human>& humans = Human::getHumans();

            if (!humans.empty())
            {
                if (
                    selectedHumanIndex < 0 ||
                    selectedHumanIndex >= static_cast<int>(humans.size())
                )
                {
                    selectedHumanIndex = 0;
                }

                int targetX;
                int targetY;
                getFacingTarget(humans[selectedHumanIndex], targetX, targetY);
                humans[selectedHumanIndex].giveManualAction(Action::attack(targetX, targetY));
            }
        }


        if (event.key.code == sf::Keyboard::Escape)
        {
            close();
            return;
        }
    }
}

void HumanInspector::moveSelectedHuman(Direction direction, GameWorld& world)
{
    std::vector<Human>& humans = Human::getHumans();

    if (humans.empty())
    {
        return;
    }

    if (
        selectedHumanIndex < 0 ||
        selectedHumanIndex >= static_cast<int>(humans.size())
    )
    {
        selectedHumanIndex = 0;
    }

    Human& human = humans[selectedHumanIndex];

    if (human.isDead())
    {
        return;
    }

    human.giveManualAction(Action::move(direction));
}

void HumanInspector::updateInspectedHumanId()
{
    std::vector<Human>& humans = Human::getHumans();

    if (humans.empty())
    {
        Human::clearInspectedHuman();
        return;
    }

    if (selectedHumanIndex < 0)
    {
        selectedHumanIndex = 0;
    }

    if (selectedHumanIndex >= static_cast<int>(humans.size()))
    {
        selectedHumanIndex = 0;
    }

    for (int i = 0; i < static_cast<int>(humans.size()); ++i)
    {
        if (i == selectedHumanIndex)
        {
            humans[i].setManualBrain();
        }
        else if (!humans[i].isDead())
        {
            humans[i].setRandomBrain();
        }
    }

    Human::setInspectedHumanId(humans[selectedHumanIndex].getId());
}


void HumanInspector::moveToNextHuman()
{
    std::vector<Human>& humans = Human::getHumans();

    if (humans.empty())
    {
        Human::clearInspectedHuman();
        return;
    }

    selectedHumanIndex++;

    if (selectedHumanIndex >= static_cast<int>(humans.size()))
    {
        selectedHumanIndex = 0;
    }

    updateInspectedHumanId();
}

void HumanInspector::moveToPreviousHuman()
{
    std::vector<Human>& humans = Human::getHumans();

    if (humans.empty())
    {
        return;
    }

    selectedHumanIndex--;

    if (selectedHumanIndex < 0)
    {
        selectedHumanIndex = static_cast<int>(humans.size()) - 1;
    }

    updateInspectedHumanId();
}

void HumanInspector::draw(GameWorld& world)
{
    if (!window.isOpen())
    {
        Human::clearInspectedHuman();
        return;
    }

    handleEvents(world);

    if (!window.isOpen())
    {
        Human::clearInspectedHuman();
        return;
    }

    window.clear(sf::Color(255, 244, 200));

    drawPanelBackground();

    std::vector<Human>& humans = Human::getHumans();

    if (humans.empty())
    {
        Human::clearInspectedHuman();

        drawCenteredText(
            "No humans exist.",
            Config::INSPECTOR_WINDOW_WIDTH / 2.0f,
            60.0f,
            24
        );

        window.display();
        return;
    }

    if (selectedHumanIndex >= static_cast<int>(humans.size()))
    {
        selectedHumanIndex = 0;
    }

    const Human& human = humans[selectedHumanIndex];

    Human::setInspectedHumanId(human.getId());

    drawVision(human, world);
    drawInventory(human);

    float centerX = Config::INSPECTOR_WINDOW_WIDTH / 2.0f;

    float neuralWidth = Config::INSPECTOR_WINDOW_WIDTH - 110.0f;
    float neuralHeight = 250.0f;
    float neuralX = centerX - neuralWidth / 2.0f;
    float neuralY = 345.0f;

    drawNeuralNetworkBox(neuralX, neuralY, neuralWidth, neuralHeight);
    drawStats(human);

    window.display();
}

void HumanInspector::drawInventory(const Human& human)
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

    const std::vector<TileType>& inventory = human.getInventory();

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

void HumanInspector::drawPanelBackground()
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

void HumanInspector::drawVision(const Human& human, const GameWorld& world)
{
    int tileSize = Config::INSPECTOR_VIEW_TILE_SIZE;

    float centerX = Config::INSPECTOR_WINDOW_WIDTH / 2.0f;

    int humanDrawX = static_cast<int>(centerX - tileSize / 2.0f);
    int humanDrawY = 250;

    drawCenteredText("Human View", centerX, 55.0f, 22);

    std::vector<VisibleTile> visibleTiles = human.getVisibleTiles(world);

    for (int forward = 1; forward <= Config::HUMAN_VISION_RANGE; ++forward)
    {
        int sideLimit = forward - 1;

        for (int side = -sideLimit; side <= sideLimit; ++side)
        {
            int drawX = humanDrawX + side * tileSize;
            int drawY = humanDrawY - forward * tileSize;

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
        int drawX = humanDrawX + tile.side * tileSize;
        int drawY = humanDrawY - tile.forward * tileSize;

        sf::RectangleShape square;
        square.setSize(sf::Vector2f(tileSize - 2.0f, tileSize - 2.0f));
        square.setPosition(drawX, drawY);
        square.setFillColor(getColorFromTile(tile.tile));
        square.setOutlineColor(sf::Color::Black);
        square.setOutlineThickness(1);

        window.draw(square);
    }

    sf::RectangleShape humanSquare;
    humanSquare.setSize(sf::Vector2f(tileSize, tileSize));
    humanSquare.setPosition(humanDrawX, humanDrawY);
    humanSquare.setFillColor(sf::Color::White);
    humanSquare.setOutlineColor(sf::Color::Black);
    humanSquare.setOutlineThickness(2);

    window.draw(humanSquare);
}

void HumanInspector::drawStats(const Human& human)
{
    float centerX = Config::INSPECTOR_WINDOW_WIDTH / 2.0f;

    float leftX = centerX - 150.0f;
    float rightX = centerX + 10.0f;

    float y = 610.0f;
    float spacing = 26.0f;

    unsigned int statTextSize = 18;

    drawText(
        "Human ID: " + std::to_string(human.getId()),
        leftX,
        y,
        statTextSize
    );

    drawText(
        "Age: " + std::to_string(human.getAge()),
        leftX,
        y + spacing,
        statTextSize
    );

    drawText(
        "Health: " + std::to_string(human.getHealth()),
        leftX,
        y + spacing * 2.0f,
        statTextSize
    );

    drawText(
        "Thirst: " + std::to_string(human.getThirst()),
        leftX,
        y + spacing * 3.0f,
        statTextSize
    );

    drawText(
        "Hunger: " + std::to_string(human.getHunger()),
        rightX,
        y,
        statTextSize
    );

    drawText(
        "Gender: " + human.getGenderString(),
        rightX,
        y + spacing,
        statTextSize
    );

    drawText(
        "Orientation: " + human.getOrientationString(),
        rightX,
        y + spacing * 2.0f,
        statTextSize
    );

    std::string status = human.isDead() ? "Dead" : "Alive";

    drawText(
        "Status: " + status,
        rightX,
        y + spacing * 3.0f,
        statTextSize
    );
}

void HumanInspector::drawNeuralNetworkBox(
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

void HumanInspector::drawText(
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

void HumanInspector::drawCenteredText(
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

sf::Color HumanInspector::getColorFromTile(TileType tile) const
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

        case TileType::Crop:
            color = Config::COLOR_FOOD;
            break;

        case TileType::Human:
            color = Config::COLOR_HUMAN;
            break;

        case TileType::Sand:
            color = Config::COLOR_SAND;
            break;

        case TileType::Predator:
            color = Config::COLOR_PREDATOR;
            break;

        case TileType::Empty:
        default:
            color = Config::COLOR_BACKGROUND;
            break;
    }

    return DayNight::apply(color);
}