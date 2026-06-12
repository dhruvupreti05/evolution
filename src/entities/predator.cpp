#include "entities/predator.h"
#include "core/config.h"
#include "environment/lake.h"
#include "entities/brain.h"
#include "entities/action.h"
#include "entities/human.h"

#include <cstdlib>
#include <memory>
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

            if (tile == TileType::Crop)
            {
                continue;
            }

            if (Human::isBlockingEntityAt(candidateX, candidateY))
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

            if (Human::isBlockingEntityAt(candidateX, candidateY))
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
    : Entity(
          gridX,
          gridY,
          Config::PREDATOR_START_HEALTH,
          Config::PREDATOR_START_THIRST,
          Config::PREDATOR_START_HUNGER
      ),
      type(type)
{
    setBrain(std::make_unique<PredatorSmartBrain>());
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

    Entity::update(world);
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

bool Predator::canMoveTo(GameWorld& world, int targetX, int targetY) const
{
    if (!world.isInsideGrid(targetX, targetY))
    {
        return false;
    }

    if (Human::isBlockingEntityAt(targetX, targetY))
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

bool Predator::tryMove(Direction direction, GameWorld& world)
{
    int dx = 0;
    int dy = 0;

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
            return false;
    }

    int targetX = x + dx;
    int targetY = y + dy;

    if (!canMoveTo(world, targetX, targetY))
    {
        return false;
    }

    x = targetX;
    y = targetY;
    return true;
}

bool Predator::tryEatAt(GameWorld& world, int targetX, int targetY)
{
    if (!isFourNeighborDistance(x, y, targetX, targetY))
    {
        return false;
    }

    Human* body = Human::getAdjacentEdibleBody(x, y);

    if (body == nullptr)
    {
        return false;
    }

    if (body->getX() != targetX || body->getY() != targetY)
    {
        return false;
    }

    body->eatBodyOneTick();

    increaseHunger(
        Config::HUNGER_PER_TICK_MEAL_HUMAN,
        Config::PREDATOR_START_HUNGER
    );

    return true;
}

bool Predator::tryDrinkAt(GameWorld& world, int targetX, int targetY)
{
    if (!isFourNeighborDistance(x, y, targetX, targetY))
    {
        return false;
    }

    if (!Lake::drinkWaterAt(world, targetX, targetY))
    {
        return false;
    }

    increaseThirst(
        Config::PREDATOR_THIRST_PER_TICK_WATER,
        Config::PREDATOR_START_THIRST
    );

    return true;
}

bool Predator::tryAttackAt(GameWorld& world, int targetX, int targetY)
{
    if (!isFourNeighborDistance(x, y, targetX, targetY))
    {
        return false;
    }

    Human* human = Human::getAdjacentLivingHuman(x, y);

    if (human == nullptr)
    {
        return false;
    }

    if (human->getX() != targetX || human->getY() != targetY)
    {
        return false;
    }

    human->takeDamage(Config::HEALTH_PER_PREDATOR_ATTACK);
    return true;
}