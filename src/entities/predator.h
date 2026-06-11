#pragma once

#include <vector>

#include "core/gameworld.h"
#include "entities/player.h"

enum class PredatorType
{
    Land,
    Water
};

class Predator
{
public:
    Predator(int gridX, int gridY, PredatorType type);

    void update(GameWorld& world);
    void draw(GameWorld& world) const;

    static void init(GameWorld& world);
    static void updatePredators(GameWorld& world);
    static void drawPredators(GameWorld& world);

    static bool isPredatorAt(int x, int y);

    int getX() const;
    int getY() const;

private:
    static std::vector<Predator> predators;

    int x;
    int y;

    int health;
    int thirst;
    int hunger;

    PredatorType type;
    bool dead = false;

    void decayStats();
    void checkDeath();

    bool isLandPredator() const;
    bool isWaterPredator() const;

    bool isThirstMode() const;

    void updateHungerMode(GameWorld& world);
    void updateThirstMode(GameWorld& world);
    void moveRandomly(GameWorld& world);

    bool tryEatAdjacentBody();
    bool tryAttackAdjacentLivingHuman();

    bool tryDrinkAdjacentWater(GameWorld& world);
    void moveToward(GameWorld& world, int targetX, int targetY);

    bool canMoveTo(GameWorld& world, int targetX, int targetY) const;
};