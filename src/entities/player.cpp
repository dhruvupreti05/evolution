#include "player.h"
#include "core/config.h"

#include <cstdlib>
#include <set>
#include <utility>

std::vector<Player> Player::players;
int Player::nextId = 0;
int Player::inspectedPlayerId = -1;

Player::Player(int gridX, int gridY) : x(gridX),
      y(gridY),
      health(Config::HUMAN_START_HEALTH + (rand() % (2 * Config::HUMAN_START_HEALTH_BUFFER + 1) - Config::HUMAN_START_HEALTH_BUFFER)),
      thirst(Config::HUMAN_START_THIRST + (rand() % (2 * Config::HUMAN_START_THIRST_BUFFER + 1) - Config::HUMAN_START_THIRST_BUFFER)),
      hunger(Config::HUMAN_START_HUNGER + (rand() % (2 * Config::HUMAN_START_HUNGER_BUFFER + 1) - Config::HUMAN_START_HUNGER_BUFFER)),
      age(0),
      gender((rand() % 2 == 0) ? Gender::Male : Gender::Female),
      id(nextId++)
{
}

void Player::init(GameWorld& world)
{
    for (int i = 0; i < Config::NUM_PLAYERS; ++i)
    {
        int gridX;
        int gridY;

        do
        {
            gridX = rand() % world.getGridWidth();
            gridY = rand() % world.getGridHeight();
        }
        while (world.getTile(gridX, gridY) == TileType::Water);

        players.emplace_back(gridX, gridY);
    }
}

void Player::update(GameWorld& world)
{
    if (dead)
    {
        return;
    }

    decayStats();
    checkDeath();

    if (dead)
    {
        return;
    }

    if (id == inspectedPlayerId)
    {
        return;
    }

    moveRandomly(world);
}

void Player::decayStats()
{
    health -= Config::HUMAN_HEALTH_DECAY;
    thirst -= Config::HUMAN_THIRST_DECAY;
    hunger -= Config::HUMAN_HUNGER_DECAY;
    age += Config::HUMAN_AGE_INCREASE;

    if (health < 0)
    {
        health = 0;
    }

    if (thirst < 0)
    {
        thirst = 0;
    }

    if (hunger < 0)
    {
        hunger = 0;
    }
}

void Player::checkDeath()
{
    if (health <= 0 || thirst <= 0 || hunger <= 0)
    {
        dead = true;
    }
}

void Player::recordMoveAttempt(MoveAttempt attempt)
{
    previousMoves[moveMemoryIndex] = attempt;

    moveMemoryIndex = (moveMemoryIndex + 1) % MOVE_MEMORY_SIZE;

    if (moveMemoryCount < MOVE_MEMORY_SIZE)
    {
        moveMemoryCount++;
    }
}

MoveAttempt Player::getPreviousMove() const
{
    int previousIndex = moveMemoryIndex - 1;

    if (previousIndex < 0)
    {
        previousIndex = MOVE_MEMORY_SIZE - 1;
    }

    return previousMoves[previousIndex];
}

Direction Player::getRandomDirection() const
{
    int choice = rand() % 5;

    switch (choice)
    {
        case 0:
            return Direction::Stay;

        case 1:
            return Direction::Up;

        case 2:
            return Direction::Down;

        case 3:
            return Direction::Left;

        case 4:
            return Direction::Right;

        default:
            return Direction::Stay;
    }
}

void Player::directionToDelta(Direction direction, int& dx, int& dy) const
{
    dx = 0;
    dy = 0;

    switch (direction)
    {
        case Direction::Up:
            dy = -1;
            break;

        case Direction::Down:
            dy = 1;
            break;

        case Direction::Left:
            dx = -1;
            break;

        case Direction::Right:
            dx = 1;
            break;

        case Direction::Stay:
        default:
            break;
    }
}

void Player::updateOrientation(Direction direction)
{
    switch (direction)
    {
        case Direction::Up:
            orientation = Orientation::North;
            break;

        case Direction::Down:
            orientation = Orientation::South;
            break;

        case Direction::Left:
            orientation = Orientation::West;
            break;

        case Direction::Right:
            orientation = Orientation::East;
            break;

        case Direction::Stay:
        default:
            break;
    }
}

void Player::move(Direction direction, GameWorld& world)
{
    if (dead)
    {
        return;
    }

    if (direction != Direction::Stay)
    {
        updateOrientation(direction);
    }

    int dx;
    int dy;

    directionToDelta(direction, dx, dy);

    int newX = x + dx;
    int newY = y + dy;

    MoveAttempt attempt;
    attempt.direction = direction;
    attempt.tile = TileType::Empty;
    attempt.succeeded = false;

    if (!world.isInsideGrid(newX, newY))
    {
        recordMoveAttempt(attempt);
        return;
    }

    TileType tile = world.getTile(newX, newY);
    attempt.tile = tile;

    if (direction == Direction::Stay)
    {
        attempt.succeeded = true;
        recordMoveAttempt(attempt);
        return;
    }

    if (tile == TileType::Water)
    {
        bool shouldMoveThroughWater = false;

        if (moveMemoryCount > 0)
        {
            MoveAttempt previousMove = getPreviousMove();

            shouldMoveThroughWater =
                previousMove.direction == direction &&
                previousMove.tile == TileType::Water &&
                previousMove.succeeded == false;
        }

        if (shouldMoveThroughWater)
        {
            x = newX;
            y = newY;

            attempt.succeeded = true;
        }

        recordMoveAttempt(attempt);
        return;
    }

    x = newX;
    y = newY;

    attempt.succeeded = true;
    recordMoveAttempt(attempt);
}

void Player::moveRandomly(GameWorld& world)
{
    Direction direction = getRandomDirection();

    move(direction, world);
}

void Player::orientationToBasis(
    int& forwardDx,
    int& forwardDy,
    int& rightDx,
    int& rightDy
) const
{
    forwardDx = 0;
    forwardDy = 0;
    rightDx = 0;
    rightDy = 0;

    switch (orientation)
    {
        case Orientation::North:
            forwardDx = 0;
            forwardDy = -1;
            rightDx = 1;
            rightDy = 0;
            break;

        case Orientation::South:
            forwardDx = 0;
            forwardDy = 1;
            rightDx = -1;
            rightDy = 0;
            break;

        case Orientation::East:
            forwardDx = 1;
            forwardDy = 0;
            rightDx = 0;
            rightDy = 1;
            break;

        case Orientation::West:
            forwardDx = -1;
            forwardDy = 0;
            rightDx = 0;
            rightDy = -1;
            break;
    }
}

bool Player::shouldLogVisibleTile(TileType tile) const
{
    return tile != TileType::Empty && tile != TileType::Sand;
}

std::vector<VisibleTile> Player::getVisibleTiles(const GameWorld& world) const
{
    std::vector<VisibleTile> visibleTiles;

    if (dead)
    {
        return visibleTiles;
    }

    int forwardDx;
    int forwardDy;
    int rightDx;
    int rightDy;

    orientationToBasis(forwardDx, forwardDy, rightDx, rightDy);

    // First: see world tiles like water, food, trees, etc.
    for (int forward = 1; forward <= Config::HUMAN_VISION_RANGE; ++forward)
    {
        int sideLimit = forward - 1;

        for (int side = -sideLimit; side <= sideLimit; ++side)
        {
            int worldX = x + forward * forwardDx + side * rightDx;
            int worldY = y + forward * forwardDy + side * rightDy;

            if (!world.isInsideGrid(worldX, worldY))
            {
                continue;
            }

            TileType tile = world.getTile(worldX, worldY);

            if (!shouldLogVisibleTile(tile))
            {
                continue;
            }

            visibleTiles.push_back({
                tile,
                forward,
                side,
                worldX,
                worldY
            });
        }
    }

    // Second: see other humans.
    for (const auto& otherPlayer : players)
    {
        if (otherPlayer.isDead())
        {
            continue;
        }

        if (otherPlayer.getId() == id)
        {
            continue;
        }

        int dx = otherPlayer.getX() - x;
        int dy = otherPlayer.getY() - y;

        int forward = dx * forwardDx + dy * forwardDy;
        int side = dx * rightDx + dy * rightDy;

        if (forward < 1 || forward > Config::HUMAN_VISION_RANGE)
        {
            continue;
        }

        int sideLimit = forward - 1;

        if (side < -sideLimit || side > sideLimit)
        {
            continue;
        }

        visibleTiles.push_back({
            TileType::Human,
            forward,
            side,
            otherPlayer.getX(),
            otherPlayer.getY()
        });
    }

    return visibleTiles;
}

void Player::draw(GameWorld& world) const
{
    if (dead)
    {
        return;
    }

    if (id == inspectedPlayerId)
    {
        drawVisionOutline(world);
        world.drawTile(x, y, Config::COLOR_SELECTED_HUMAN);
    }
    else
    {
        world.drawTile(x, y, TileType::Human);
    }
}

void Player::updatePlayers(GameWorld& world)
{
    for (auto& player : players)
    {
        player.update(world);
    }
}

void Player::movePlayers(GameWorld& world)
{
    for (auto& player : players)
    {
        player.moveRandomly(world);
    }
}

void Player::drawPlayers(GameWorld& world)
{
    int selectedId = Player::getInspectedPlayerId();

    for (const auto& player : players)
    {
        if (player.getId() != selectedId)
        {
            player.draw(world);
        }
    }

    for (const auto& player : players)
    {
        if (player.getId() == selectedId)
        {
            player.draw(world);
        }
    }
}

std::vector<Player>& Player::getPlayers()
{
    return players;
}

void Player::setInspectedPlayerId(int playerId)
{
    inspectedPlayerId = playerId;
}

void Player::clearInspectedPlayer()
{
    inspectedPlayerId = -1;
}

int Player::getX() const
{
    return x;
}

int Player::getY() const
{
    return y;
}

int Player::getHealth() const
{
    return health;
}

int Player::getThirst() const
{
    return thirst;
}

int Player::getHunger() const
{
    return hunger;
}

int Player::getAge() const
{
    return age;
}

int Player::getId() const
{
    return id;
}

Gender Player::getGender() const
{
    return gender;
}

Orientation Player::getOrientation() const
{
    return orientation;
}

std::string Player::getGenderString() const
{
    switch (gender)
    {
        case Gender::Male:
            return "Male";

        case Gender::Female:
            return "Female";

        default:
            return "Unknown";
    }
}

std::string Player::getOrientationString() const
{
    switch (orientation)
    {
        case Orientation::North:
            return "North";

        case Orientation::South:
            return "South";

        case Orientation::East:
            return "East";

        case Orientation::West:
            return "West";

        default:
            return "Unknown";
    }
}

bool Player::isDead() const
{
    return dead;
}

int Player::getInspectedPlayerId()
{
    return inspectedPlayerId;
}

void Player::drawVisionOutline(GameWorld& world) const
{
    if (dead)
    {
        return;
    }

    int forwardDx;
    int forwardDy;
    int rightDx;
    int rightDy;

    orientationToBasis(forwardDx, forwardDy, rightDx, rightDy);

    std::set<std::pair<int, int>> visionCells;

    for (int forward = 1; forward <= Config::HUMAN_VISION_RANGE; ++forward)
    {
        int sideLimit = forward - 1;

        for (int side = -sideLimit; side <= sideLimit; ++side)
        {
            int worldX = x + forward * forwardDx + side * rightDx;
            int worldY = y + forward * forwardDy + side * rightDy;

            if (!world.isInsideGrid(worldX, worldY))
            {
                continue;
            }

            visionCells.insert({worldX, worldY});
        }
    }

    float cellSize = static_cast<float>(world.getCellSize());

    for (const auto& cell : visionCells)
    {
        int gx = cell.first;
        int gy = cell.second;

        bool hasUp    = visionCells.count({gx, gy - 1}) > 0;
        bool hasDown  = visionCells.count({gx, gy + 1}) > 0;
        bool hasLeft  = visionCells.count({gx - 1, gy}) > 0;
        bool hasRight = visionCells.count({gx + 1, gy}) > 0;

        float px = gx * cellSize;
        float py = gy * cellSize;

        if (!hasUp)
        {
            world.drawLine(px, py, px + cellSize, py, sf::Color::Black, 2.0f);
        }

        if (!hasDown)
        {
            world.drawLine(
                px,
                py + cellSize,
                px + cellSize,
                py + cellSize,
                sf::Color::Black,
                2.0f
            );
        }

        if (!hasLeft)
        {
            world.drawLine(px, py, px, py + cellSize, sf::Color::Black, 2.0f);
        }

        if (!hasRight)
        {
            world.drawLine(
                px + cellSize,
                py,
                px + cellSize,
                py + cellSize,
                sf::Color::Black,
                2.0f
            );
        }
    }
}

void Player::controlledMove(Direction direction, GameWorld& world)
{
    move(direction, world);
}
