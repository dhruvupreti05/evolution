#include "core/debuglog.h"

#include <iostream>

namespace
{
    // File-only switch that controls whether debug messages are printed.
    bool debugEnabled = false;
}

/*
    Turns debug logging on or off for the whole simulation.
*/
void DebugLog::setEnabled(bool enabled)
{
    debugEnabled = enabled;
}

/*
    Returns whether debug logging is currently active.
*/
bool DebugLog::isEnabled()
{
    return debugEnabled;
}

/*
    Prints a general debug message if logging is enabled.
*/
void DebugLog::message(const std::string& text)
{
    if (!debugEnabled)
    {
        return;
    }

    std::cout << "[Debug] " << text << '\n';
}

/*
    Prints the action chosen by an entity during a simulation step.
*/
void DebugLog::action(const std::string& entityType, int entityId, const Action& action)
{
    if (!debugEnabled)
    {
        return;
    }

    std::cout << "[Action] " << entityType << " " << entityId << " -> " << action.toString() << '\n';
}

/*
    Prints when a new entity is created from two parents.
*/
void DebugLog::birth(const std::string& entityType, int parentAId, int parentBId, int childId)
{
    if (!debugEnabled)
    {
        return;
    }

    std::cout << "[Birth] " << entityType << " child " << childId << " from parents " << parentAId << " and " << parentBId << '\n';
}

/*
    Prints when an entity dies and includes the reason.
*/
void DebugLog::death(const std::string& entityType, int entityId, const std::string& reason)
{
    if (!debugEnabled)
    {
        return;
    }

    std::cout << "[Death] " << entityType << " " << entityId << ": " << reason << '\n';
}