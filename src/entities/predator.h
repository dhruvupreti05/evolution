#pragma once

#include <vector>

#include "core/gameworld.h"
#include "entities/entity.h"
#include "entities/direction.h"

/*
    Type of predator.
    Land predators stay on land, while water predators are meant to live on water tiles.
*/
enum class PredatorType
{
    Land,
    Water
};

/*
    Predator entity in the simulation.
    Predators can move, hunt humans, eat dead bodies, drink, mate, and die if their survival stats run out.
*/
class Predator : public Entity
{
public:
    Predator(int gridX, int gridY, PredatorType type);

    static Predator* getById(int id);

    void update(GameWorld& world) override;
    void draw(GameWorld& world) const override;

    bool tryMove(Direction direction, GameWorld& world) override;
    bool tryEatAt(GameWorld& world, int targetX, int targetY) override;
    bool tryDrinkAt(GameWorld& world, int targetX, int targetY) override;
    bool tryAttackAt(GameWorld& world, int targetX, int targetY) override;

    static void init(GameWorld& world);
    static void updatePredators(GameWorld& world);
    static void drawPredators(GameWorld& world);
    static void drawBodies(GameWorld& world);

    static bool isPredatorAt(int x, int y);

    bool hasBody() const;

    static int countAlive();
    static int countDead();

    static void killWaterPredatorsNotOnWater(GameWorld& world);

    static const std::vector<Predator>& getPredators();
    static std::vector<Predator>& getPredatorsMutable();

    bool tryMateAt(GameWorld& world, int targetX, int targetY) override;

    int getId() const;
    PredatorType getType() const;

    static void resolveMatingActions(GameWorld& world);
    static Predator* getAdjacentLivingPredator(int x, int y, const Predator* self = nullptr);

private:
    static int nextId;
    int id;

    static std::vector<Predator> predators;

    // Dead predators leave a body behind for a limited number of ticks.
    int deadBodyTicksRemaining = 0;

    PredatorType type;

    static bool findChildSpawnCell(GameWorld& world, const Predator& parentA, const Predator& parentB, PredatorType childType, int& childX, int& childY);

    void decayStats() override;
    void checkDeath() override;

    bool isLandPredator() const;
    bool isWaterPredator() const;

    // Applies the land/water movement rules for this predator type.
    bool canMoveTo(GameWorld& world, int targetX, int targetY) const;
};