#pragma once

#include <vector>

#include "core/gameworld.h"

class Human;
class Predator;

class EntityOccupancy
{
public:
    static void rebuild(GameWorld& world);

    static bool hasBeenBuilt();

    static Human* getHumanAt(int x, int y);
    static Predator* getPredatorAt(int x, int y);

    static bool hasHumanAt(int x, int y);
    static bool hasPredatorAt(int x, int y);
    static bool isBlockedAt(int x, int y);

    static void updateHumanPosition(Human& human, int oldX, int oldY);
    static void updatePredatorPosition(Predator& predator, int oldX, int oldY);

private:
    static bool built;
    static int gridWidth;

    static int makeKey(int x, int y);
    static bool isValidKey(int key, int vectorSize);

    static std::vector<Human*> humansByPosition;
    static std::vector<Predator*> predatorsByPosition;
};