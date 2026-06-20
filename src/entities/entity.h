#pragma once

#include <memory>
#include <vector>

#include "core/gameworld.h"
#include "entities/direction.h"
#include "entities/action.h"
#include "entities/crop.h"

class Brain;

/*
    Base class for living things in the simulation.
    It stores shared survival stats, position, brain behavior, prepared actions, mating cooldowns, and child tracking.
*/
class Entity
{
public:
    Entity(int gridX, int gridY, int startHealth, int startThirst, int startHunger);

    virtual ~Entity() = default;

    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;

    virtual void update(GameWorld& world);
    virtual void draw(GameWorld& world) const = 0;

    int getX() const;
    int getY() const;
    int getHealth() const;
    int getThirst() const;
    int getHunger() const;

    bool isDead() const;

    void setBrain(std::unique_ptr<Brain> newBrain);
    Brain* getBrain();

    void takeDamage(int amount, GameWorld& world);
    void increaseHunger(int amount, int maxHunger);
    void increaseThirst(int amount, int maxThirst);

    virtual bool tryMove(Direction direction, GameWorld& world) = 0;
    virtual bool tryEatAt(GameWorld& world, int targetX, int targetY) = 0;
    virtual bool tryDrinkAt(GameWorld& world, int targetX, int targetY) = 0;
    virtual bool tryAttackAt(GameWorld& world, int targetX, int targetY) = 0;
    virtual bool tryMateAt(GameWorld& world, int targetX, int targetY);
    virtual bool tryPickUp(GameWorld& world);
    virtual bool tryDrop(GameWorld& world);

    void prepareAction(GameWorld& world);
    void executePreparedAction(GameWorld& world);
    const Action& getPreparedAction() const;
    bool hasPreparedAction() const;
    void clearPreparedAction();

    void addChild(int childId);
    const std::vector<int>& getChildren() const;

    bool canMateNow() const;
    void startMatingCooldown(int ticks);
    void tickMatingCooldown();
    int getMatingCooldownTicksRemaining() const;

protected:
    int x;
    int y;

    // Prevents the same entity from mating again immediately.
    int matingCooldownTicksRemaining = 0;

    int health;
    int thirst;
    int hunger;

    virtual void decayStats() = 0;
    virtual void checkDeath(GameWorld& world) = 0;

    bool dead = false;

    // Owns the decision-making object used by this entity.
    std::unique_ptr<Brain> brain;

    // Stores the action chosen for this tick before it is actually executed.
    Action preparedAction = Action::stay();
    bool preparedActionReady = false;

    // Stores child IDs so family/lineage information can be inspected later.
    std::vector<int> children;

    void executeAction(const Action& action, GameWorld& world);
};