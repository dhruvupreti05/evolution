#include "entities/predator.h"

#include "core/config.h"
#include "terrain/lake.h"

#include <cstdlib>
#include <cmath>

std::vector<Predator> Predator::predators;

namespace
{
    int manhattanDistance(int x1, int y1, int x2, int y2)
    {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

    bool isFourNeighborDistance(int x1, int y1, int x2, int y2)
    {
        return manhattanDistance(x1, y1, x2, y2) == 1;
    }

    bool findRandomLandSpawn(GameWorld& world, int& x, int& y)
    {
        for (int attempt = 0; attempt < 5000; ++attempt)
        {
            int candidateX = rand() % world.getGridWidth();
            int candidateY = rand() % world.getGridHeight();

            TileType tile = world.getTile(candidateX, candidateY);

            if (tile == TileType::Water)
            {
                continue;
            }

            if (tile == TileType::Food)
            {
                continue;
            }

            if (Player::isBlockingEntityAt(candidateX, candidateY))
            {
                continue;
            }

            x = candidateX;
            y = candidateY;
            return true;
        }

        return false;
    }

    bool findRandomWaterSpawn(GameWorld& world, int& x, int& y)
    {
        for (int attempt = 0; attempt < 5000; ++attempt)
        {
            int candidateX = rand() % world.getGridWidth();
            int candidateY = rand() % world.getGridHeight();

            if (world.getTile(candidateX, candidateY) != TileType::Water)
            {
                continue;
            }

            if (Player::isBlockingEntityAt(candidateX, candidateY))
            {
                continue;
            }

            x = candidateX;
            y = candidateY;
            return true;
        }

        return false;
    }
}

Predator::Predator(int gridX, int gridY, PredatorType type)
    : x(gridX),
      y(gridY),
      health(Config::PREDATOR_START_HEALTH),
      thirst(Config::PREDATOR_START_THIRST),
      hunger(Config::PREDATOR_START_HUNGER),
      type(type)
{
}

void Predator::init(GameWorld& world)
{
    for (int i = 0; i < Config::NUMBER_OF_LAND_PREDATORS; ++i)
    {
        int x;
        int y;

        if (findRandomLandSpawn(world, x, y))
        {
            predators.emplace_back(x, y, PredatorType::Land);
        }
    }

    for (int i = 0; i < Config::NUMBER_OF_WATER_PREDATORS; ++i)
    {
        int x;
        int y;

        if (findRandomWaterSpawn(world, x, y))
        {
            predators.emplace_back(x, y, PredatorType::Water);
        }
    }
}

void Predator::updatePredators(GameWorld& world)
{
    for (auto& predator : predators)
    {
        predator.update(world);
    }
}

void Predator::drawPredators(GameWorld& world)
{
    for (const auto& predator : predators)
    {
        predator.draw(world);
    }
}

void Predator::update(GameWorld& world)
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

    if (isThirstMode())
    {
        updateThirstMode(world);
        return;
    }

    updateHungerMode(world);
}

bool Predator::isDead() const
{
    return dead;
}

bool Predator::hasBody() const
{
    return dead && deadBodyTicksRemaining > 0;
}

void Predator::drawBodies(GameWorld& world)
{
    for (const auto& predator : predators)
    {
        if (predator.hasBody())
        {
            world.drawTile(
                predator.getX(),
                predator.getY(),
                Config::COLOR_DEAD_PREDATOR
            );
        }
    }
}

void Predator::decayStats()
{
    health -= Config::PREDATOR_HEALTH_DECAY;
    thirst -= Config::PREDATOR_THIRST_DECAY;
    hunger -= Config::PREDATOR_HUNGER_DECAY;

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

void Predator::checkDeath()
{
    if (dead)
    {
        return;
    }

    if (health <= 0 || thirst <= 0 || hunger <= 0)
    {
        dead = true;
        deadBodyTicksRemaining = Config::TICKS_PER_DEAD_PREDATOR;
    }
}

bool Predator::isLandPredator() const
{
    return type == PredatorType::Land;
}

bool Predator::isWaterPredator() const
{
    return type == PredatorType::Water;
}

bool Predator::isThirstMode() const
{
    return thirst <= Config::PREDATOR_THIRST_MODE_THRESHOLD;
}

void Predator::updateHungerMode(GameWorld& world)
{
    if (tryEatAdjacentBody())
    {
        return;
    }

    if (tryAttackAdjacentLivingHuman())
    {
        return;
    }

    Player* target = Player::getNearestLivingPlayerOrBody(x, y);

    if (target == nullptr)
    {
        moveRandomly(world);
        return;
    }

    moveToward(world, target->getX(), target->getY());
}

void Predator::updateThirstMode(GameWorld& world)
{
    if (tryDrinkAdjacentWater(world))
    {
        return;
    }

    int bestX = -1;
    int bestY = -1;
    int bestDistance = 0;

    for (int yy = 0; yy < world.getGridHeight(); ++yy)
    {
        for (int xx = 0; xx < world.getGridWidth(); ++xx)
        {
            if (world.getTile(xx, yy) != TileType::Water)
            {
                continue;
            }

            int distance = manhattanDistance(x, y, xx, yy);

            if (bestX == -1 || distance < bestDistance)
            {
                bestX = xx;
                bestY = yy;
                bestDistance = distance;
            }
        }
    }

    if (bestX == -1)
    {
        moveRandomly(world);
        return;
    }

    moveToward(world, bestX, bestY);
}

bool Predator::tryEatAdjacentBody()
{
    Player* body = Player::getAdjacentEdibleBody(x, y);

    if (body == nullptr)
    {
        return false;
    }

    body->claimBodyForEating();
    body->eatBodyOneTick();

    hunger += Config::HUNGER_PER_TICK_MEAL_HUMAN;

    if (hunger > Config::PREDATOR_START_HUNGER)
    {
        hunger = Config::PREDATOR_START_HUNGER;
    }

    return true;
}

bool Predator::tryAttackAdjacentLivingHuman()
{
    Player* player = Player::getAdjacentLivingPlayer(x, y);

    if (player == nullptr)
    {
        return false;
    }

    player->takeDamage(Config::HEALTH_PER_PREDATOR_ATTACK);

    return true;
}

bool Predator::tryDrinkAdjacentWater(GameWorld& world)
{
    const int directions[4][2] = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1}
    };

    for (const auto& direction : directions)
    {
        int waterX = x + direction[0];
        int waterY = y + direction[1];

        if (!world.isInsideGrid(waterX, waterY))
        {
            continue;
        }

        if (world.getTile(waterX, waterY) != TileType::Water)
        {
            continue;
        }

        if (!Lake::removeWaterAt(world, waterX, waterY))
        {
            continue;
        }

        thirst += Config::PREDATOR_THIRST_PER_WATER_BLOCK;

        if (thirst > Config::PREDATOR_START_THIRST)
        {
            thirst = Config::PREDATOR_START_THIRST;
        }

        return true;
    }

    return false;
}

void Predator::moveRandomly(GameWorld& world)
{
    int choice = rand() % 5;

    int dx = 0;
    int dy = 0;

    switch (choice)
    {
        case 1:
            dy = -1;
            break;

        case 2:
            dy = 1;
            break;

        case 3:
            dx = -1;
            break;

        case 4:
            dx = 1;
            break;

        case 0:
        default:
            break;
    }

    int targetX = x + dx;
    int targetY = y + dy;

    if (canMoveTo(world, targetX, targetY))
    {
        x = targetX;
        y = targetY;
    }
}

void Predator::moveToward(GameWorld& world, int targetX, int targetY)
{
    int bestX = x;
    int bestY = y;
    int bestDistance = manhattanDistance(x, y, targetX, targetY);

    const int directions[4][2] = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1}
    };

    for (const auto& direction : directions)
    {
        int candidateX = x + direction[0];
        int candidateY = y + direction[1];

        if (!canMoveTo(world, candidateX, candidateY))
        {
            continue;
        }

        int candidateDistance =
            manhattanDistance(candidateX, candidateY, targetX, targetY);

        if (candidateDistance < bestDistance)
        {
            bestDistance = candidateDistance;
            bestX = candidateX;
            bestY = candidateY;
        }
    }

    x = bestX;
    y = bestY;
}

bool Predator::canMoveTo(GameWorld& world, int targetX, int targetY) const
{
    if (!world.isInsideGrid(targetX, targetY))
    {
        return false;
    }

    if (Player::isBlockingEntityAt(targetX, targetY))
    {
        return false;
    }

    if (Predator::isPredatorAt(targetX, targetY))
    {
        return false;
    }

    TileType tile = world.getTile(targetX, targetY);

    if (isLandPredator() && tile == TileType::Water)
    {
        return false;
    }

    if (isWaterPredator() && tile != TileType::Water)
    {
        return false;
    }

    return true;
}

void Predator::draw(GameWorld& world) const
{
    if (dead)
    {
        return;
    }

    world.drawTile(x, y, Config::COLOR_PREDATOR);
}

bool Predator::isPredatorAt(int x, int y)
{
    for (const auto& predator : predators)
    {
        if (predator.dead)
        {
            if (predator.hasBody() && predator.x == x && predator.y == y)
            {
                return true;
            }

            continue;
        }

        if (predator.x == x && predator.y == y)
        {
            return true;
        }
    }

    return false;
}

int Predator::getX() const
{
    return x;
}

int Predator::getY() const
{
    return y;
}

int Predator::countAlive()
{
    int count = 0;

    for (const auto& predator : predators)
    {
        if (!predator.dead)
        {
            count++;
        }
    }

    return count;
}

int Predator::countDead()
{
    int count = 0;

    for (const auto& predator : predators)
    {
        if (predator.dead)
        {
            count++;
        }
    }

    return count;
}

void Predator::killWaterPredatorsNotOnWater(GameWorld& world)
{
    for (auto& predator : predators)
    {
        if (predator.dead)
        {
            continue;
        }

        if (!predator.isWaterPredator())
        {
            continue;
        }

        if (!world.isInsideGrid(predator.x, predator.y))
        {
            predator.dead = true;
            predator.deadBodyTicksRemaining = Config::TICKS_PER_DEAD_PREDATOR;
            continue;
        }

        if (world.getTile(predator.x, predator.y) != TileType::Water)
        {
            predator.dead = true;
            predator.deadBodyTicksRemaining = Config::TICKS_PER_DEAD_PREDATOR;
        }
    }
}

const std::vector<Predator>& Predator::getPredators()
{
    return predators;
}