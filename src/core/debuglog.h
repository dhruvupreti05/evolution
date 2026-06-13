#pragma once

#include <string>

#include "entities/action.h"

namespace DebugLog
{
    void setEnabled(bool enabled);
    bool isEnabled();

    void message(const std::string& text);
    void action(const std::string& entityType, int entityId, const Action& action);
    void birth(
        const std::string& entityType,
        int parentAId,
        int parentBId,
        int childId
    );
    void death(const std::string& entityType, int entityId, const std::string& reason);
}