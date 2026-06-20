#include "brain/brain.h"
#include "brain/predator-smart-brain.h"
#include "entities/predator.h"
#include "entities/entity.h"
#include "entities/human.h"
#include "entities/crop.h"
#include "core/config.h"
#include "entities/body.h"
#include "core/gridutils.h"
#include "environment/lake.h"

#include <cstdlib>

/*
    The decision function for the smart predator.
    It sometimes tries to mate, switches to water seeking when thirsty,
    and otherwise focuses on hunting or eating humans.
*/
Action PredatorSmartBrain::chooseAction(Entity& entity, GameWorld& world)
{
    Predator* predator = dynamic_cast<Predator*>(&entity);

    if (predator != nullptr && predator->canMateNow() && rand() % 20 == 0)
    {
        Predator* mate = Predator::getAdjacentLivingPredator(predator->getX(), predator->getY(), predator);

        if (mate != nullptr)
        {
            return Action::mate(mate->getX(), mate->getY());
        }
    }

    // Thirst takes priority once it drops below the predator's safe threshold.
    if (entity.getThirst() <= Config::PREDATOR_THIRST_MODE_THRESHOLD)
    {
        return chooseThirstAction(entity, world);
    }

    return chooseHungerAction(entity, world);
}

/*
    Chooses what the predator should do when it needs water.
    It drinks immediately if water is adjacent, otherwise it moves toward
    the nearest water cell it can find.
*/
Action PredatorSmartBrain::chooseThirstAction(Entity& entity, GameWorld& world)
{
    for (const auto& direction : GridUtils::FOUR_DIRECTIONS)
    {
        int waterX = entity.getX() + direction.dx;
        int waterY = entity.getY() + direction.dy;

        if (!world.isInsideGrid(waterX, waterY))
        {
            continue;
        }

        if (world.getTile(waterX, waterY) == TileType::Water)
        {
            return Action::drink(waterX, waterY);
        }
    }

    int waterX;
    int waterY;

    bool foundWater = Lake::getNearestWaterCellWithinRange(world, entity.getX(), entity.getY(), Config::PREDATOR_WATER_SEARCH_RANGE, waterX, waterY);

    if (!foundWater)
    {
        return Action::move(GridUtils::randomDirection());
    }

    return Action::move(
        GridUtils::directionToward(
            entity.getX(),
            entity.getY(),
            waterX,
            waterY
        )
    );
}

/*
    Chooses predator hunger behavior.
    Predators eat adjacent bodies first, attack adjacent living humans second, then move toward the nearest body or living human.
*/
Action PredatorSmartBrain::chooseHungerAction(Entity& entity, GameWorld& world)
{
    Body* body = Body::getAdjacentEdibleBody(entity.getX(), entity.getY());

    if (body != nullptr)
    {
        return Action::eat(body->getX(), body->getY());
    }

    Human* livingHuman = Human::getAdjacentLivingHuman(entity.getX(), entity.getY());

    if (livingHuman != nullptr)
    {
        return Action::attack(livingHuman->getX(), livingHuman->getY());
    }

    Body* bodyTarget = Body::getNearestBodyWithinRange(world, entity.getX(), entity.getY(), Config::PREDATOR_PREY_SEARCH_RANGE);

    if (bodyTarget != nullptr)
    {
        return Action::move(GridUtils::directionToward(entity.getX(), entity.getY(), bodyTarget->getX(), bodyTarget->getY()));
    }

    Human* humanTarget = Human::getNearestLivingHumanWithinRange(world, entity.getX(), entity.getY(), Config::PREDATOR_PREY_SEARCH_RANGE);

    if (humanTarget == nullptr)
    {
        return Action::move(GridUtils::randomDirection());
    }

    return Action::move(GridUtils::directionToward(entity.getX(), entity.getY(), humanTarget->getX(), humanTarget->getY()));
}