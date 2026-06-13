#include "ui/humaninspector.h"

#include "core/config.h"
#include "environment/daynight.h"
#include "core/tilecolors.h"
#include "ui/uidrawhelper.h"

#include <vector>
#include <string>
#include <cmath>

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

    resetAllHumanBrains();
    Human::clearInspectedHuman();
}

bool HumanInspector::isOpen() const
{
    return window.isOpen();
}

void HumanInspector::resetAllHumanBrains()
{
    std::vector<Human>& humans = Human::getHumans();

    for (Human& human : humans)
    {
        if (!human.isDead())
        {
            human.setRandomBrain();
        }
    }
}

Human* HumanInspector::getSelectedHuman()
{
    std::vector<Human>& humans = Human::getHumans();

    if (humans.empty())
    {
        return nullptr;
    }

    if (
        selectedHumanIndex < 0 ||
        selectedHumanIndex >= static_cast<int>(humans.size())
    )
    {
        selectedHumanIndex = 0;
    }

    if (humans[selectedHumanIndex].isDead())
    {
        return nullptr;
    }

    return &humans[selectedHumanIndex];
}

void HumanInspector::updateInspectedHumanId()
{
    std::vector<Human>& humans = Human::getHumans();

    if (humans.empty())
    {
        Human::clearInspectedHuman();
        return;
    }

    if (
        selectedHumanIndex < 0 ||
        selectedHumanIndex >= static_cast<int>(humans.size())
    )
    {
        selectedHumanIndex = 0;
    }

    resetAllHumanBrains();

    Human* selectedHuman = getSelectedHuman();

    if (selectedHuman == nullptr)
    {
        Human::clearInspectedHuman();
        return;
    }

    selectedHuman->setManualBrain();
    Human::setInspectedHumanId(selectedHuman->getId());
}

void HumanInspector::moveToNextHuman()
{
    std::vector<Human>& humans = Human::getHumans();

    if (humans.empty())
    {
        Human::clearInspectedHuman();
        return;
    }

    int humanCount = static_cast<int>(humans.size());

    for (int attempts = 0; attempts < humanCount; ++attempts)
    {
        selectedHumanIndex++;

        if (selectedHumanIndex >= humanCount)
        {
            selectedHumanIndex = 0;
        }

        if (!humans[selectedHumanIndex].isDead())
        {
            updateInspectedHumanId();
            return;
        }
    }

    Human::clearInspectedHuman();
}

void HumanInspector::moveToPreviousHuman()
{
    std::vector<Human>& humans = Human::getHumans();

    if (humans.empty())
    {
        Human::clearInspectedHuman();
        return;
    }

    int humanCount = static_cast<int>(humans.size());

    for (int attempts = 0; attempts < humanCount; ++attempts)
    {
        selectedHumanIndex--;

        if (selectedHumanIndex < 0)
        {
            selectedHumanIndex = humanCount - 1;
        }

        if (!humans[selectedHumanIndex].isDead())
        {
            updateInspectedHumanId();
            return;
        }
    }

    Human::clearInspectedHuman();
}

void HumanInspector::moveSelectedHuman(Direction direction)
{
    Human* human = getSelectedHuman();

    if (human == nullptr)
    {
        return;
    }

    human->giveManualAction(Action::move(direction));
}

void HumanInspector::eatFacingTile()
{
    Human* human = getSelectedHuman();

    if (human == nullptr)
    {
        return;
    }

    int targetX;
    int targetY;

    getFacingTarget(*human, targetX, targetY);

    human->giveManualAction(Action::eat(targetX, targetY));
}

void HumanInspector::drinkFacingTile()
{
    Human* human = getSelectedHuman();

    if (human == nullptr)
    {
        return;
    }

    int targetX;
    int targetY;

    getFacingTarget(*human, targetX, targetY);

    human->giveManualAction(Action::drink(targetX, targetY));
}

void HumanInspector::attackFacingTile()
{
    Human* human = getSelectedHuman();

    if (human == nullptr)
    {
        return;
    }

    int targetX;
    int targetY;

    getFacingTarget(*human, targetX, targetY);

    human->giveManualAction(Action::attack(targetX, targetY));
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
        if (event.key.code == sf::Keyboard::Escape)
        {
            close();
            return;
        }
        else if (event.key.code == sf::Keyboard::M)
        {
            mateFacingEntity();
        }
        else if (event.key.code == sf::Keyboard::Right)
        {
            moveToNextHuman();
        }
        else if (event.key.code == sf::Keyboard::Left)
        {
            moveToPreviousHuman();
        }
        else if (event.key.code == sf::Keyboard::W)
        {
            moveSelectedHuman(Direction::Up);
        }
        else if (event.key.code == sf::Keyboard::A)
        {
            moveSelectedHuman(Direction::Left);
        }
        else if (event.key.code == sf::Keyboard::S)
        {
            moveSelectedHuman(Direction::Down);
        }
        else if (event.key.code == sf::Keyboard::D)
        {
            moveSelectedHuman(Direction::Right);
        }
        else if (event.key.code == sf::Keyboard::E)
        {
            eatFacingTile();
        }
        else if (event.key.code == sf::Keyboard::R)
        {
            drinkFacingTile();
        }
        else if (event.key.code == sf::Keyboard::F)
        {
            attackFacingTile();
        }
        else if (event.key.code == sf::Keyboard::C)
        {
            pickUpItem();
        }
        else if (event.key.code == sf::Keyboard::V)
        {
            dropItem();
        }
    }
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

    if (
        selectedHumanIndex < 0 ||
        selectedHumanIndex >= static_cast<int>(humans.size())
    )
    {
        selectedHumanIndex = 0;
    }

    if (humans[selectedHumanIndex].isDead())
    {
        moveToNextHuman();
    }

    Human* selectedHuman = getSelectedHuman();

    if (selectedHuman == nullptr)
    {
        drawCenteredText(
            "No living humans to inspect.",
            Config::INSPECTOR_WINDOW_WIDTH / 2.0f,
            60.0f,
            24
        );

        window.display();
        return;
    }

    const Human& human = *selectedHuman;

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
    float titleInventorySpace = 15.0f;

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
        float slotY = startY + titleInventorySpace;

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
    const float tileSize = static_cast<float>(Config::INSPECTOR_VIEW_TILE_SIZE);

    const float centerX = Config::INSPECTOR_WINDOW_WIDTH / 2.0f;

    const float humanDrawX = std::round(centerX - tileSize / 2.0f);
    const float humanDrawY = 250.0f;

    drawCenteredText("Human View", centerX, 55.0f, 22);

    std::vector<VisibleTile> visibleTiles = human.getVisibleTiles(world);

    auto drawTile = [&](float x,
                        float y,
                        sf::Color fillColor,
                        sf::Color outlineColor,
                        float outlineThickness)
    {
        sf::RectangleShape square;

        square.setPosition(
            std::round(x + outlineThickness),
            std::round(y + outlineThickness)
        );

        square.setSize(sf::Vector2f(
            tileSize - outlineThickness * 2.0f,
            tileSize - outlineThickness * 2.0f
        ));

        square.setFillColor(fillColor);
        square.setOutlineColor(outlineColor);
        square.setOutlineThickness(outlineThickness);

        window.draw(square);
    };

    for (int forward = 1; forward <= Config::HUMAN_VISION_RANGE; ++forward)
    {
        int sideLimit = forward - 1;

        for (int side = -sideLimit; side <= sideLimit; ++side)
        {
            float drawX = humanDrawX + side * tileSize;
            float drawY = humanDrawY - forward * tileSize;

            drawTile(
                drawX,
                drawY,
                Config::COLOR_BACKGROUND,
                sf::Color(215, 215, 215),
                1.0f
            );
        }
    }

    for (const auto& tile : visibleTiles)
    {
        float drawX = humanDrawX + tile.side * tileSize;
        float drawY = humanDrawY - tile.forward * tileSize;

        drawTile(
            drawX,
            drawY,
            getColorFromTile(tile.tile),
            sf::Color::Black,
            1.0f
        );
    }

    drawTile(
        humanDrawX,
        humanDrawY,
        sf::Color::White,
        sf::Color::Black,
        2.0f
    );
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
    UiDrawHelper::drawText(window, font, fontLoaded, text, x, y, size);
}

void HumanInspector::drawCenteredText(
    const std::string& text,
    float centerX,
    float y,
    unsigned int size
)
{
    UiDrawHelper::drawCenteredText(
        window,
        font,
        fontLoaded,
        text,
        centerX,
        y,
        size
    );
}

sf::Color HumanInspector::getColorFromTile(TileType tile) const
{
    return TileColors::getDayNight(tile);
}

void HumanInspector::pickUpItem()
{
    Human* human = getSelectedHuman();

    if (human == nullptr)
    {
        return;
    }

    human->giveManualAction(Action::pickUp());
}

void HumanInspector::dropItem()
{
    Human* human = getSelectedHuman();

    if (human == nullptr)
    {
        return;
    }

    human->giveManualAction(Action::drop());
}

void HumanInspector::mateFacingEntity()
{
    Human* human = getSelectedHuman();

    if (human == nullptr)
    {
        return;
    }

    int targetX;
    int targetY;

    getFacingTarget(*human, targetX, targetY);

    human->giveManualAction(Action::mate(targetX, targetY));

    std::vector<Human>& humans = Human::getHumans();

    for (Human& other : humans)
    {
        if (other.getId() == human->getId())
        {
            continue;
        }

        if (other.isDead())
        {
            continue;
        }

        if (other.getX() == targetX && other.getY() == targetY)
        {
            if (human->canMateWith(other))
            {
                other.giveManualAction(Action::mate(human->getX(), human->getY()));
            }

            return;
        }
    }
}