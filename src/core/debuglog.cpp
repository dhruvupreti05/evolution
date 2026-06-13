#include "core/debuglog.h"

#include <iostream>

namespace
{
    bool debugEnabled = false;
}

void DebugLog::setEnabled(bool enabled)
{
    debugEnabled = enabled;
}

bool DebugLog::isEnabled()
{
    return debugEnabled;
}

void DebugLog::message(const std::string& text)
{
    if (!debugEnabled)
    {
        return;
    }

    std::cout << "[Debug] " << text << '\n';
}

void DebugLog::action(
    const std::string& entityType,
    int entityId,
    const Action& action
)
{
    if (!debugEnabled)
    {
        return;
    }

    std::cout
        << "[Action] "
        << entityType
        << " "
        << entityId
        << " -> "
        << action.toString()
        << '\n';
}

void DebugLog::birth(
    const std::string& entityType,
    int parentAId,
    int parentBId,
    int childId
)
{
    if (!debugEnabled)
    {
        return;
    }

    std::cout
        << "[Birth] "
        << entityType
        << " child "
        << childId
        << " from parents "
        << parentAId
        << " and "
        << parentBId
        << '\n';
}

void DebugLog::death(
    const std::string& entityType,
    int entityId,
    const std::string& reason
)
{
    if (!debugEnabled)
    {
        return;
    }

    std::cout
        << "[Death] "
        << entityType
        << " "
        << entityId
        << ": "
        << reason
        << '\n';
}