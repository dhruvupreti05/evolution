#pragma once

#include <array>
#include <vector>
#include <string>

#include "core/gameworld.h"

enum class Direction
{
    Stay,
    Up,
    Down,
    Left,
    Right
};

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

class Player
{
public:
    Player(int gridX, int gridY);

    void update(GameWorld& world);
    void moveRandomly(GameWorld& world);
    void draw(GameWorld& world) const;

    static void init(GameWorld& world);
    static void updatePlayers(GameWorld& world);
    static void drawPlayers(GameWorld& world);
    static void movePlayers(GameWorld& world);

    static std::vector<Player>& getPlayers();

    static void setInspectedPlayerId(int playerId);
    static int getInspectedPlayerId();
    static void clearInspectedPlayer();

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

    bool tryPickUp(GameWorld& world);
    bool tryDrop(GameWorld& world);

    const std::vector<TileType>& getInventory() const;

    static bool isPlayerAt(int x, int y);

    void takeDamage(int amount);

    bool hasBody() const;
    bool isBodyEdible() const;
    bool isBodyClaimedThisTick() const;

    void claimBodyForEating();
    void eatBodyOneTick();

    static void drawBodies(GameWorld& world);
    static void resetBodyEatingClaims();

    static bool isBlockingEntityAt(int x, int y);
    static Player* getAdjacentLivingPlayer(int x, int y);
    static Player* getAdjacentEdibleBody(int x, int y);
    static Player* getNearestLivingPlayerOrBody(int x, int y);

    static int countAlive();
    static int countDead();

private:
    int deadBodyTicksRemaining = 0;
    int bodyMealTicksRemaining = 0;
    bool bodyClaimedThisTick = false;
    
    static std::vector<Player> players;
    static int nextId;
    static int inspectedPlayerId;

    static constexpr int MOVE_MEMORY_SIZE = 10;

    int x;
    int y;

    Orientation orientation = Orientation::South;

    std::array<MoveAttempt, MOVE_MEMORY_SIZE> previousMoves;
    int moveMemoryIndex = 0;
    int moveMemoryCount = 0;

    int health;
    int thirst;
    int hunger;
    int age;

    Gender gender;
    int id;

    bool dead = false;

    void move(Direction direction, GameWorld& world);
    void decayStats();
    void checkDeath();

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