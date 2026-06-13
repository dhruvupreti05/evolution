#pragma once

#include <vector>

class Human;
class Predator;

class EntityOccupancy
{
public:
    static void rebuild();

    static bool hasBeenBuilt();

    static Human* getHumanAt(int x, int y);
    static Predator* getPredatorAt(int x, int y);

    static bool hasHumanAt(int x, int y);
    static bool hasPredatorAt(int x, int y);
    static bool isBlockedAt(int x, int y);

private:
    static bool built;

    static int makeKey(int x, int y);

    static std::vector<Human*> humansByPosition;
    static std::vector<Predator*> predatorsByPosition;

    static int maxKey;
    static void ensureKeyExists(int key);
};