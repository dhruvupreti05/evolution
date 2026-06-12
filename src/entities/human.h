#pragma once

#include <array>
#include <vector>
#include <string>

#include "core/gameworld.h"
#include "entities/entity.h"
#include "entities/food.h"
#include "entities/direction.h"

enum class Orientation
{
    North,
    South,
    East,
    West
};

enum class Gender
{
    Male,
    Female
};

struct MoveAttempt
{
    Direction direction = Direction::Stay;
    TileType tile = TileType::Empty;
    bool succeeded = false;
};

struct VisibleTile
{
    TileType tile = TileType::Empty;

    int forward = 0;
    int side = 0;

    int worldX = 0;
    int worldY = 0;
};
struct Action;


class Human : public Entity, public Food
{
public:
    Human(int gridX, int gridY);
    bool isEdible() const override;

    bool tryMove(Direction direction, GameWorld& world) override;
    bool tryEatAt(GameWorld& world, int targetX, int targetY) override;
    bool tryDrinkAt(GameWorld& world, int targetX, int targetY) override;
    bool tryAttackAt(GameWorld& world, int targetX, int targetY) override;

    void setManualBrain();
    void setRandomBrain();
    void giveManualAction(const Action& action);


    void moveRandomly(GameWorld& world);

    void update(GameWorld& world) override;
    void draw(GameWorld& world) const override;

    static void init(GameWorld& world);
    static void updateHumans(GameWorld& world);
    static void drawHumans(GameWorld& world);
    static void moveHumans(GameWorld& world);

    static std::vector<Human>& getHumans();

    static void setInspectedHumanId(int humanId);
    static int getInspectedHumanId();
    static void clearInspectedHuman();

    int getX() const;
    int getY() const;

    int getHealth() const;
    int getThirst() const;
    int getHunger() const;
    int getAge() const;
    int getId() const;

    Gender getGender() const;
    Orientation getOrientation() const;

    std::string getGenderString() const;
    std::string getOrientationString() const;

    bool isDead() const;

    std::vector<VisibleTile> getVisibleTiles(const GameWorld& world) const;

    void controlledMove(Direction direction, GameWorld& world);

    bool tryPickUp(GameWorld& world) override;
    bool tryDrop(GameWorld& world) override;

    const std::vector<TileType>& getInventory() const;

    static bool isHumanAt(int x, int y);

    bool hasBody() const;
    bool isBodyEdible() const;
    bool isBodyClaimedThisTick() const;

    void claimBodyForEating();
    void eatBodyOneTick();

    static void drawBodies(GameWorld& world);
    static void resetBodyEatingClaims();

    static bool isBlockingEntityAt(int x, int y);
    static Human* getAdjacentLivingHuman(int x, int y);
    static Human* getAdjacentEdibleBody(int x, int y);
    static Human* getNearestLivingHumanOrBody(int x, int y);

    static int countAlive();
    static int countDead();

    bool tryMateAt(GameWorld& world, int targetX, int targetY) override;

    static void resolveMatingActions(GameWorld& world);
    static Human* getAdjacentMateCandidate(const Human& human);

    bool canMateWith(const Human& other) const;

private:
    int deadBodyTicksRemaining = 0;
    
    static std::vector<Human> humans;
    static int nextId;
    static int inspectedHumanId;

    static constexpr int MOVE_MEMORY_SIZE = 10;

    Orientation orientation = Orientation::South;

    std::array<MoveAttempt, MOVE_MEMORY_SIZE> previousMoves;
    int moveMemoryIndex = 0;
    int moveMemoryCount = 0;

    int age;

    Gender gender;
    int id;

    void move(Direction direction, GameWorld& world);

    void decayStats() override;
    void checkDeath() override;

    static bool findChildSpawnCell(
        GameWorld& world,
        const Human& parentA,
        const Human& parentB,
        int& childX,
        int& childY
    );

    void recordMoveAttempt(MoveAttempt attempt);
    MoveAttempt getPreviousMove() const;

    Direction getRandomDirection() const;
    void directionToDelta(Direction direction, int& dx, int& dy) const;

    void updateOrientation(Direction direction);
    void orientationToBasis(
        int& forwardDx,
        int& forwardDy,
        int& rightDx,
        int& rightDy
    ) const;

    void drawVisionOutline(GameWorld& world) const;
    bool shouldLogVisibleTile(TileType tile) const;

    std::vector<TileType> inventory;

    void getFacingCell(int& targetX, int& targetY) const;
    bool canPickUp(TileType tile) const;
    bool canDropOn(TileType tile, int targetX, int targetY) const;
};