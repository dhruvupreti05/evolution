#pragma once

#include <array>
#include <vector>

#include "gameworld.h"

enum class Direction
{
    Stay,
    Up,
    Down,
    Left,
    Right
};

struct MoveAttempt
{
    Direction direction = Direction::Stay;
    TileType tile = TileType::Empty;
    bool succeeded = false;
};

class Player
{
public:
    Player(int gridX, int gridY);

    void moveRandomly(GameWorld& world);
    void draw(GameWorld& world) const;

    static void init(GameWorld& world);
    static void drawPlayers(GameWorld& world);
    static void movePlayers(GameWorld& world);

    int getX() const;
    int getY() const;

private:
    static std::vector<Player> players;

    static constexpr int MOVE_MEMORY_SIZE = 10;

    int x;
    int y;

    std::array<MoveAttempt, MOVE_MEMORY_SIZE> previousMoves;
    int moveMemoryIndex = 0;
    int moveMemoryCount = 0;

    Direction pendingWaterDirection = Direction::Stay;
    int pendingWaterAttempts = 0;

    void move(Direction direction, GameWorld& world);
    void recordMoveAttempt(MoveAttempt attempt);

    Direction getRandomDirection() const;
    void directionToDelta(Direction direction, int& dx, int& dy) const;
};