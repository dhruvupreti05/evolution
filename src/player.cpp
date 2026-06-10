#include "player.h"
#include "config.h"

#include <cstdlib>

std::vector<Player> Player::players;

Player::Player(int gridX, int gridY)
    : x(gridX), y(gridY)
{
}

void Player::init(GameWorld& world)
{
    for (int i = 0; i < Config::NUM_PLAYERS; ++i)
    {
        int gridX = rand() % world.getGridSize();
        int gridY = rand() % world.getGridSize();

        players.emplace_back(gridX, gridY);
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

void Player::move(Direction direction, GameWorld& world)
{
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
        pendingWaterDirection = Direction::Stay;
        pendingWaterAttempts = 0;

        recordMoveAttempt(attempt);
        return;
    }

    TileType tile = world.getTile(newX, newY);
    attempt.tile = tile;

    if (direction == Direction::Stay)
    {
        pendingWaterDirection = Direction::Stay;
        pendingWaterAttempts = 0;

        attempt.succeeded = true;
        recordMoveAttempt(attempt);
        return;
    }

    if (tile == TileType::Water)
    {
        if (pendingWaterDirection == direction)
        {
            pendingWaterAttempts++;
        }
        else
        {
            pendingWaterDirection = direction;
            pendingWaterAttempts = 1;
        }

        if (pendingWaterAttempts >= 2)
        {
            x = newX;
            y = newY;

            pendingWaterDirection = Direction::Stay;
            pendingWaterAttempts = 0;

            attempt.succeeded = true;
        }

        recordMoveAttempt(attempt);
        return;
    }

    pendingWaterDirection = Direction::Stay;
    pendingWaterAttempts = 0;

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

void Player::draw(GameWorld& world) const
{
    world.drawTile(x, y, TileType::Human);
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
    for (const auto& player : players)
    {
        player.draw(world);
    }
}

int Player::getX() const
{
    return x;
}

int Player::getY() const
{
    return y;
}