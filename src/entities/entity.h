#pragma once

#include <memory>

#include "core/gameworld.h"
#include "entities/direction.h"

class Brain;
struct Action;

class Entity
{
public:
    Entity(
        int gridX,
        int gridY,
        int startHealth,
        int startThirst,
        int startHunger
    );

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

    virtual void takeDamage(int amount);
    void increaseHunger(int amount, int maxHunger);
    void increaseThirst(int amount, int maxThirst);

    virtual bool tryMove(Direction direction, GameWorld& world) = 0;
    virtual bool tryEatAt(GameWorld& world, int targetX, int targetY) = 0;
    virtual bool tryDrinkAt(GameWorld& world, int targetX, int targetY) = 0;
    virtual bool tryAttackAt(GameWorld& world, int targetX, int targetY) = 0;

protected:
    int x;
    int y;

    int health;
    int thirst;
    int hunger;

    bool dead = false;

    std::unique_ptr<Brain> brain;

    void executeAction(const Action& action, GameWorld& world);

    virtual void decayStats() = 0;
    virtual void checkDeath() = 0;
};
