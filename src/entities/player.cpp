#include "player.h"
#include "core/config.h"
#include "terrain/lake.h"
#include "resources/food.h"
#include "entities/predator.h"

#include <cstdlib>
#include <set>
#include <utility>
#include <cmath>

std::vector<Player> Player::players;
int Player::nextId = 0;
int Player::inspectedPlayerId = -1;

static bool isFourNeighborDistance(int x1, int y1, int x2, int y2)
{
    int dx = std::abs(x1 - x2);
    int dy = std::abs(y1 - y2);

    return dx + dy == 1;
}

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
        while ( world.getTile(gridX, gridY) == TileType::Water || Player::isPlayerAt(gridX, gridY));

        players.emplace_back(gridX, gridY);
    }
}

void Player::update(GameWorld& world)
{
    if (dead)
    {
        if (deadBodyTicksRemaining > 0)
        {
            deadBodyTicksRemaining--;
        }

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

void Player::takeDamage(int amount)
{
    if (dead)
    {
        return;
    }

    health -= amount;

    if (health < 0)
    {
        health = 0;
    }

    checkDeath();
}

bool Player::hasBody() const
{
    return dead && deadBodyTicksRemaining > 0 && bodyMealTicksRemaining > 0;
}

bool Player::isBodyEdible() const
{
    return hasBody();
}

bool Player::isBodyClaimedThisTick() const
{
    return bodyClaimedThisTick;
}

void Player::claimBodyForEating()
{
    bodyClaimedThisTick = true;
}

void Player::eatBodyOneTick()
{
    if (!hasBody())
    {
        return;
    }

    bodyMealTicksRemaining--;

    if (bodyMealTicksRemaining < 0)
    {
        bodyMealTicksRemaining = 0;
    }
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
    if (dead)
    {
        return;
    }

    if (health <= 0 || thirst <= 0 || hunger <= 0)
    {
        dead = true;
        deadBodyTicksRemaining = Config::TICKS_PER_DEAD_BODY;
        bodyMealTicksRemaining = Config::TICKS_PER_MEAL_HUMAN;
        bodyClaimedThisTick = false;
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

    if (direction != Direction::Stay && Player::isBlockingEntityAt(newX, newY))
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

    // Third: see predators.
    // This is after world tiles, so water predators draw over water.
    for (const auto& predator : Predator::getPredators())
    {
        if (predator.isDead())
        {
            continue;
        }

        int dx = predator.getX() - x;
        int dy = predator.getY() - y;

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
            TileType::Predator,
            forward,
            side,
            predator.getX(),
            predator.getY()
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

void Player::drawBodies(GameWorld& world)
{
    for (const auto& player : players)
    {
        if (player.hasBody())
        {
            world.drawTile(player.getX(), player.getY(), Config::COLOR_DEAD_BODY);
        }
    }
}

void Player::resetBodyEatingClaims()
{
    for (auto& player : players)
    {
        player.bodyClaimedThisTick = false;
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

void Player::getFacingCell(int& targetX, int& targetY) const
{
    int forwardDx;
    int forwardDy;
    int rightDx;
    int rightDy;

    orientationToBasis(forwardDx, forwardDy, rightDx, rightDy);

    targetX = x + forwardDx;
    targetY = y + forwardDy;
}

bool Player::canPickUp(TileType tile) const
{
    return tile == TileType::Water || tile == TileType::Food;
}

bool Player::canDropOn(TileType tile, int targetX, int targetY) const
{
    if (tile != TileType::Empty && tile != TileType::Sand)
    {
        return false;
    }

    if (Player::isPlayerAt(targetX, targetY))
    {
        return false;
    }

    return true;
}

bool Player::tryPickUp(GameWorld& world)
{
    if (dead)
    {
        return false;
    }

    if (static_cast<int>(inventory.size()) >= Config::HUMAN_INVENTORY_SIZE)
    {
        return false;
    }

    int targetX;
    int targetY;
    getFacingCell(targetX, targetY);

    if (!world.isInsideGrid(targetX, targetY))
    {
        return false;
    }

    TileType tile = world.getTile(targetX, targetY);

    if (!canPickUp(tile))
    {
        return false;
    }

    if (tile == TileType::Food)
    {
        Food::removeFoodAt(targetX, targetY);
        world.setTile(targetX, targetY, TileType::Empty);
        inventory.push_back(TileType::Food);
        return true;
    }

    if (tile == TileType::Water)
    {
        bool removed = Lake::removeWaterAt(world, targetX, targetY);

        if (!removed)
        {
            return false;
        }

        inventory.push_back(TileType::Water);
        return true;
    }

    return false;
}

bool Player::tryDrop(GameWorld& world)
{
    if (dead)
    {
        return false;
    }

    if (inventory.empty())
    {
        return false;
    }

    int targetX;
    int targetY;
    getFacingCell(targetX, targetY);

    if (!world.isInsideGrid(targetX, targetY))
    {
        return false;
    }

    TileType targetTile = world.getTile(targetX, targetY);

    if (!canDropOn(targetTile, targetX, targetY))
    {
        return false;
    }

    TileType item = inventory.back();

    if (item == TileType::Food)
    {
        world.setTile(targetX, targetY, TileType::Food);
        Food::addFoodAt(targetX, targetY);
        inventory.pop_back();
        return true;
    }

    if (item == TileType::Water)
    {
        bool placed = Lake::placeWaterAt(world, targetX, targetY);

        if (!placed)
        {
            return false;
        }

        inventory.pop_back();
        return true;
    }

    return false;
}

const std::vector<TileType>& Player::getInventory() const
{
    return inventory;
}

bool Player::isPlayerAt(int x, int y)
{
    for (const auto& player : players)
    {
        if (player.isDead())
        {
            if (player.hasBody() && player.getX() == x && player.getY() == y)
            {
                return true;
            }

            continue;
        }

        if (player.getX() == x && player.getY() == y)
        {
            return true;
        }
    }

    return false;
}

bool Player::isBlockingEntityAt(int x, int y)
{
    for (const auto& player : players)
    {
        if (player.isDead())
        {
            if (player.hasBody() && player.getX() == x && player.getY() == y)
            {
                return true;
            }

            continue;
        }

        if (player.getX() == x && player.getY() == y)
        {
            return true;
        }
    }

    if (Predator::isPredatorAt(x, y))
    {
        return true;
    }

    return false;
}

Player* Player::getAdjacentLivingPlayer(int x, int y)
{
    for (auto& player : players)
    {
        if (player.isDead())
        {
            continue;
        }

        if (isFourNeighborDistance(x, y, player.getX(), player.getY()))
        {
            return &player;
        }
    }

    return nullptr;
}

Player* Player::getAdjacentEdibleBody(int x, int y)
{
    for (auto& player : players)
    {
        if (!player.isBodyEdible())
        {
            continue;
        }

        if (player.isBodyClaimedThisTick())
        {
            continue;
        }

        if (isFourNeighborDistance(x, y, player.getX(), player.getY()))
        {
            return &player;
        }
    }

    return nullptr;
}

Player* Player::getNearestLivingPlayerOrBody(int x, int y)
{
    Player* nearest = nullptr;
    int bestDistance = 0;

    for (auto& player : players)
    {
        bool validTarget = !player.isDead() || player.isBodyEdible();

        if (!validTarget)
        {
            continue;
        }

        int dx = std::abs(player.getX() - x);
        int dy = std::abs(player.getY() - y);
        int distance = dx + dy;

        if (nearest == nullptr || distance < bestDistance)
        {
            nearest = &player;
            bestDistance = distance;
        }
    }

    return nearest;
}

int Player::countAlive()
{
    int count = 0;

    for (const auto& player : players)
    {
        if (!player.isDead())
        {
            count++;
        }
    }

    return count;
}

int Player::countDead()
{
    int count = 0;

    for (const auto& player : players)
    {
        if (player.isDead())
        {
            count++;
        }
    }

    return count;
}