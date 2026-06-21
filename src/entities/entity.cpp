#include "entities/entity.h"
#include "core/gameworld.h"
#include "brain/brain.h"

#include <utility>

/*
    Creates a base entity with its starting position and survival stats.
*/
Entity::Entity(int gridX, int gridY, int startHealth, int startThirst, int startHunger) : x(gridX), y(gridY), health(startHealth), thirst(startThirst), hunger(startHunger)
{
}

/*
    Runs one full entity update.
    The entity first chooses an action, then executes the action that was prepared.
*/
void Entity::update(GameWorld& world)
{
    tickMatingCooldown();
    prepareAction(world);
    executePreparedAction(world);
}

/*
    Chooses this entity's next action for the current tick.
    Dead entities stop immediately, and living entities decay before their brain gets to decide.
*/
void Entity::prepareAction(GameWorld& world)
{
    preparedAction = Action::stay();
    preparedActionReady = false;

    if (dead)
    {
        return;
    }

    decayStats();
    checkDeath(world);

    if (dead || brain == nullptr)
    {
        return;
    }

    preparedAction = brain->chooseAction(*this, world);
    preparedActionReady = true;
}

/*
    Performs the action that was chosen earlier in the tick.
*/
void Entity::executePreparedAction(GameWorld& world)
{
    if (!preparedActionReady || dead)
    {
        return;
    }

    executeAction(preparedAction, world);
    clearPreparedAction();
}

/*
    Returns the action currently waiting to be executed.
*/
const Action& Entity::getPreparedAction() const
{
    return preparedAction;
}

/*
    Returns whether an action has been prepared for this tick.
*/
bool Entity::hasPreparedAction() const
{
    return preparedActionReady;
}

/*
    Clears the stored action so it cannot accidentally run again.
*/
void Entity::clearPreparedAction()
{
    preparedAction = Action::stay();
    preparedActionReady = false;
}

/*
    Records a child entity ID for lineage tracking.
*/
void Entity::addChild(int childId)
{
    children.push_back(childId);
}

/*
    Returns the IDs of this entity's children.
*/
const std::vector<int>& Entity::getChildren() const
{
    return children;
}

/*
    Returns the entity's grid x position.
*/
int Entity::getX() const
{
    return x;
}

/*
    Returns the entity's grid y position.
*/
int Entity::getY() const
{
    return y;
}

/*
    Returns the entity's current health.
*/
int Entity::getHealth() const
{
    return health;
}

/*
    Returns the entity's current thirst.
*/
int Entity::getThirst() const
{
    return thirst;
}

/*
    Returns the entity's current hunger.
*/
int Entity::getHunger() const
{
    return hunger;
}

/*
    Returns whether this entity is dead.
*/
bool Entity::isDead() const
{
    return dead;
}

/*
    Replaces this entity's brain with a new one.
*/
void Entity::setBrain(std::unique_ptr<Brain> newBrain)
{
    brain = std::move(newBrain);
}

/*
    Returns the brain currently controlling this entity.
*/
Brain* Entity::getBrain()
{
    return brain.get();
}

/*
    Returns the brain currently controlling this entity; const version for read-only checks.
*/
const Brain* Entity::getBrain() const
{
    return brain.get();
}

/*
    Reduces health and checks whether the damage killed the entity.
*/
void Entity::takeDamage(int amount, GameWorld& world)
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

    checkDeath(world);
}

/*
    Restores hunger but does not let it go past the species maximum.
*/
void Entity::increaseHunger(int amount, int maxHunger)
{
    hunger += amount;

    if (hunger > maxHunger)
    {
        hunger = maxHunger;
    }
}

/*
    Restores thirst but does not let it go past the species maximum.
*/
void Entity::increaseThirst(int amount, int maxThirst)
{
    thirst += amount;

    if (thirst > maxThirst)
    {
        thirst = maxThirst;
    }
}

/*
    Sends an action to the correct behavior function.
    The specific entity type decides whether each attempted action succeeds.
*/
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

        case ActionType::Mate:
            tryMateAt(world, action.targetX, action.targetY);
            break;

        case ActionType::Stay:
        default:
            break;
    }
}

/*
    Default pickup behavior for entities that cannot carry items.
*/
bool Entity::tryPickUp(GameWorld& world)
{
    return false;
}

/*
    Default drop behavior for entities that cannot carry items.
*/
bool Entity::tryDrop(GameWorld& world)
{
    return false;
}

/*
    Default mating behavior for entities that do not implement reproduction.
*/
bool Entity::tryMateAt(GameWorld& world, int targetX, int targetY)
{
    return false;
}

/*
    Returns whether this entity is allowed to mate right now.
*/
bool Entity::canMateNow() const
{
    return matingCooldownTicksRemaining <= 0;
}

/*
    Starts the mating cooldown.
    Negative cooldowns are clamped to zero.
*/
void Entity::startMatingCooldown(int ticks)
{
    if (ticks < 0)
    {
        ticks = 0;
    }

    matingCooldownTicksRemaining = ticks;
}

/*
    Decreases the mating cooldown by one tick.
*/
void Entity::tickMatingCooldown()
{
    if (matingCooldownTicksRemaining > 0)
    {
        matingCooldownTicksRemaining--;
    }
}

/*
    Returns how many ticks remain before this entity can mate again.
*/
int Entity::getMatingCooldownTicksRemaining() const
{
    return matingCooldownTicksRemaining;
}