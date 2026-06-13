#include "entities/predator.h"
#include "core/config.h"
#include "core/gridutils.h"
#include "environment/lake.h"
#include "brain/predator-smart-brain.h"
#include "entities/action.h"
#include "core/debuglog.h"
#include "entities/human.h"
#include "entities/entityoccupancy.h"

#include <cstdlib>
#include <memory>
#include <cmath>
#include <set>

std::vector<Predator> Predator::predators;
int Predator::nextId = 0;

namespace
{
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
      type(type),
      id(nextId++)
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
        if (predator.dead)
        {
            if (predator.deadBodyTicksRemaining > 0)
            {
                predator.deadBodyTicksRemaining--;
            }

            predator.clearPreparedAction();
            continue;
        }

        predator.tickMatingCooldown();

        predator.decayStats();
        predator.checkDeath();

        if (predator.dead)
        {
            predator.clearPreparedAction();
            continue;
        }

        predator.prepareAction(world);
    }

    resolveMatingActions(world);

    for (auto& predator : predators)
    {
        if (predator.dead)
        {
            continue;
        }

        if (
            predator.hasPreparedAction() &&
            predator.getPreparedAction().type != ActionType::Mate
        )
        {
            predator.executePreparedAction(world);
        }
        else
        {
            predator.clearPreparedAction();
        }
    }
}

int Predator::getId() const
{
    return id;
}

PredatorType Predator::getType() const
{
    return type;
}

bool Predator::tryMateAt(GameWorld& world, int targetX, int targetY)
{
    return false;
}

Predator* Predator::getAdjacentLivingPredator(
    int x,
    int y,
    const Predator* self
)
{
    if (self != nullptr && !self->canMateNow())
    {
        return nullptr;
    }

    for (const auto& direction : GridUtils::FOUR_DIRECTIONS)
    {
        int targetX = x + direction.dx;
        int targetY = y + direction.dy;

        Predator* predator = EntityOccupancy::getPredatorAt(targetX, targetY);

        if (predator == nullptr)
        {
            continue;
        }

        if (predator->isDead())
        {
            continue;
        }

        if (self != nullptr && predator->getId() == self->getId())
        {
            continue;
        }

        if (!predator->canMateNow())
        {
            continue;
        }

        return predator;
    }

    if (EntityOccupancy::hasBeenBuilt())
    {
        return nullptr;
    }

    for (Predator& predator : predators)
    {
        if (predator.dead)
        {
            continue;
        }

        if (self != nullptr && predator.getId() == self->getId())
        {
            continue;
        }

        if (!predator.canMateNow())
        {
            continue;
        }

        if (GridUtils::isFourNeighborDistance(x, y, predator.getX(), predator.getY()))
        {
            return &predator;
        }
    }

    return nullptr;
}

Predator* Predator::getById(int id)
{
    for (Predator& predator : predators)
    {
        if (predator.getId() == id)
        {
            return &predator;
        }
    }

    return nullptr;
}

bool Predator::findChildSpawnCell(
    GameWorld& world,
    const Predator& parentA,
    const Predator& parentB,
    PredatorType childType,
    int& childX,
    int& childY
)
{
    const int candidates[8][2] = {
        {parentA.getX() + 1, parentA.getY()},
        {parentA.getX() - 1, parentA.getY()},
        {parentA.getX(), parentA.getY() + 1},
        {parentA.getX(), parentA.getY() - 1},
        {parentB.getX() + 1, parentB.getY()},
        {parentB.getX() - 1, parentB.getY()},
        {parentB.getX(), parentB.getY() + 1},
        {parentB.getX(), parentB.getY() - 1}
    };

    for (const auto& candidate : candidates)
    {
        int x = candidate[0];
        int y = candidate[1];

        if (!world.isInsideGrid(x, y))
        {
            continue;
        }

        if (Human::isBlockingEntityAt(x, y))
        {
            continue;
        }

        if (Predator::isPredatorAt(x, y))
        {
            continue;
        }

        TileType tile = world.getTile(x, y);

        if (childType == PredatorType::Land && tile == TileType::Water)
        {
            continue;
        }

        if (childType == PredatorType::Water && tile != TileType::Water)
        {
            continue;
        }

        childX = x;
        childY = y;
        return true;
    }

    return false;
}

void Predator::resolveMatingActions(GameWorld& world)
{
    std::set<int> alreadyMatedIds;

    auto alreadyMated = [&](int id)
    {
        return alreadyMatedIds.count(id) > 0;
    };

    int originalCount = static_cast<int>(predators.size());

    for (int i = 0; i < originalCount; ++i)
    {
        Predator& parentA = predators[i];

        if (
            parentA.dead ||
            !parentA.canMateNow() ||
            alreadyMated(parentA.getId()) ||
            !parentA.hasPreparedAction() ||
            parentA.getPreparedAction().type != ActionType::Mate
        )
        {
            continue;
        }

        for (int j = i + 1; j < originalCount; ++j)
        {
            Predator& parentB = predators[j];

            if (
                parentB.dead ||
                !parentB.canMateNow() ||
                alreadyMated(parentB.getId()) ||
                !parentB.hasPreparedAction() ||
                parentB.getPreparedAction().type != ActionType::Mate
            )
            {
                continue;
            }

            if (
                !GridUtils::isFourNeighborDistance(
                    parentA.getX(),
                    parentA.getY(),
                    parentB.getX(),
                    parentB.getY()
                )
            )
            {
                continue;
            }

            if (
                parentA.getPreparedAction().targetX != parentB.getX() ||
                parentA.getPreparedAction().targetY != parentB.getY() ||
                parentB.getPreparedAction().targetX != parentA.getX() ||
                parentB.getPreparedAction().targetY != parentA.getY()
            )
            {
                continue;
            }

            PredatorType childType =
                (rand() % 2 == 0) ? parentA.getType() : parentB.getType();

            int childX;
            int childY;

            if (!findChildSpawnCell(world, parentA, parentB, childType, childX, childY))
            {
                continue;
            }

            int parentAId = parentA.getId();
            int parentBId = parentB.getId();

            predators.emplace_back(childX, childY, childType);
            int childId = predators.back().getId();

            EntityOccupancy::rebuild(world);

            DebugLog::birth("Predator", parentAId, parentBId, childId);

            Predator* parentAAfterBirth = Predator::getById(parentAId);
            Predator* parentBAfterBirth = Predator::getById(parentBId);

            if (parentAAfterBirth != nullptr)
            {
                parentAAfterBirth->addChild(childId);
                parentAAfterBirth->startMatingCooldown(
                    Config::PREDATOR_MATING_COOLDOWN_TICKS
                );
                parentAAfterBirth->clearPreparedAction();
            }

            if (parentBAfterBirth != nullptr)
            {
                parentBAfterBirth->addChild(childId);
                parentBAfterBirth->startMatingCooldown(
                    Config::PREDATOR_MATING_COOLDOWN_TICKS
                );
                parentBAfterBirth->clearPreparedAction();
            }

            alreadyMatedIds.insert(parentAId);
            alreadyMatedIds.insert(parentBId);

            break;
        }
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
    if (EntityOccupancy::hasBeenBuilt())
    {
        if (EntityOccupancy::hasPredatorAt(x, y))
        {
            return true;
        }
    }
    else
    {
        for (const Predator& predator : predators)
        {
            if (!predator.dead && predator.x == x && predator.y == y)
            {
                return true;
            }
        }
    }

    for (const Predator& predator : predators)
    {
        if (predator.dead && predator.hasBody() && predator.x == x && predator.y == y)
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

std::vector<Predator>& Predator::getPredatorsMutable()
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

    int oldX = x;
    int oldY = y;

    x = targetX;
    y = targetY;

    EntityOccupancy::updatePredatorPosition(*this, oldX, oldY);

    return true;
}

bool Predator::tryEatAt(GameWorld& world, int targetX, int targetY)
{
    if (!GridUtils::isFourNeighborDistance(x, y, targetX, targetY))
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
    if (!GridUtils::isFourNeighborDistance(x, y, targetX, targetY))
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
    if (!GridUtils::isFourNeighborDistance(x, y, targetX, targetY))
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
