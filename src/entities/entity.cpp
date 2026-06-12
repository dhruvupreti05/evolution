#include "entities/entity.h"

#include "entities/action.h"
#include "brain/brain.h"

Entity::Entity(
    int gridX,
    int gridY,
    int startHealth,
    int startThirst,
    int startHunger
)
    : x(gridX),
      y(gridY),
      health(startHealth),
      thirst(startThirst),
      hunger(startHunger)
{
}

void Entity::update(GameWorld& world)
{
    if (dead)
    {
        return;
    }

    decayStats();
    checkDeath();

    if (dead || brain == nullptr)
    {
        return;
    }

    executeAction(brain->chooseAction(*this, world), world);
}

int Entity::getX() const { return x; }
int Entity::getY() const { return y; }
int Entity::getHealth() const { return health; }
int Entity::getThirst() const { return thirst; }
int Entity::getHunger() const { return hunger; }

bool Entity::isDead() const
{
    return dead;
}

void Entity::setBrain(std::unique_ptr<Brain> newBrain)
{
    brain = std::move(newBrain);
}

Brain* Entity::getBrain()
{
    return brain.get();
}

void Entity::takeDamage(int amount)
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

void Entity::increaseHunger(int amount, int maxHunger)
{
    hunger += amount;

    if (hunger > maxHunger)
    {
        hunger = maxHunger;
    }
}

void Entity::increaseThirst(int amount, int maxThirst)
{
    thirst += amount;

    if (thirst > maxThirst)
    {
        thirst = maxThirst;
    }
}

void Entity::executeAction(const Action& action, GameWorld& world)
{
    switch (action.type)
    {
        case ActionType::Move:
            tryMove(action.direction, world);
            break;

        case ActionType::Eat:
            tryEatAt(world, action.targetX, action.targetY);
            break;

        case ActionType::Drink:
            tryDrinkAt(world, action.targetX, action.targetY);
            break;

        case ActionType::Attack:
            tryAttackAt(world, action.targetX, action.targetY);
            break;

        case ActionType::PickUp:
            tryPickUp(world);
            break;

        case ActionType::Drop:
            tryDrop(world);
            break;

        case ActionType::Stay:
        default:
            break;
    }
}

bool Entity::tryPickUp(GameWorld& world)
{
    return false;
}

bool Entity::tryDrop(GameWorld& world)
{
    return false;
}