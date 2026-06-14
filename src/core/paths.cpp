#include "core/paths.h"

#include <stdexcept>

/*
    Searches upward from the current folder until it finds the project root.
*/
std::filesystem::path Paths::getRepoRoot()
{
    std::filesystem::path current = std::filesystem::current_path();

    while (true)
    {
        bool hasGitFolder = std::filesystem::exists(current / ".git");

        if (hasGitFolder)
        {
            return current;
        }

        if (current == current.root_path())
        {
            throw std::runtime_error("Could not find repo root.");
        }

        current = current.parent_path();
    }
}

/*
    Builds the full path to an asset inside docs/assets.
*/
std::filesystem::path Paths::getAssetPath(const std::string& relativePath)
{
    return getRepoRoot() / "docs" / "assets" / relativePath;
}