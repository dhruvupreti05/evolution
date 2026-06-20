#pragma once

#include <vector>

#include "core/gameworld.h"
#include "entities/food.h"

/*
    Records what kind of entity created a dead body.
    Predators can eat either kind, but keeping the source lets us inspect or extend body logic later.
*/
enum class BodySource
{
    Human,
    Predator
};

/*
    Shared dead-body system.
    Bodies sit visually on top of the world tile, block movement, can be eaten by predators, and restore the latest underlying tile when gone.
*/
class Body : public Food
{
public:
    Body(int gridX, int gridY, BodySource source, TileType tileUnderBody);

    static void addBodyAt(GameWorld& world, int x, int y, BodySource source);
    static void update(GameWorld& world);
    static void drawBodies(GameWorld& world);

    static bool isBodyAt(int x, int y);
    static bool isEdibleBodyAt(int x, int y);
    static bool eatBodyAt(GameWorld& world, int x, int y, int& hungerGain);

    static Body* getAdjacentEdibleBody(int x, int y);
    static Body* getNearestBodyWithinRange(const GameWorld& world, int x, int y, int range);

    static void resetEatingClaims();
    static void setTileUnderBodyAt(int x, int y, TileType tile);

    int getX() const;
    int getY() const;
    BodySource getSource() const;

private:
    int x;
    int y;

    BodySource source;
    TileType tileUnderBody;

    int ticksRemaining;
    bool claimedThisTick = false;

    bool isExpired() const;
    void restoreTile(GameWorld& world) const;

    static std::vector<Body> bodies;
};