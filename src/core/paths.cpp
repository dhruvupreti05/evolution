#include "core/paths.h"

#include <stdexcept>

std::filesystem::path Paths::getRepoRoot()
{
    std::filesystem::path current = std::filesystem::current_path();

    while (true)
    {
        bool hasGitFolder = std::filesystem::exists(current / ".git");
        bool hasMakefile = std::filesystem::exists(current / "Makefile");
        bool hasSrcFolder = std::filesystem::exists(current / "src");

        if (hasGitFolder || (hasMakefile && hasSrcFolder))
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

std::filesystem::path Paths::getAssetPath(const std::string& relativePath)
{
    return getRepoRoot() / "docs" / "assets" / relativePath;
}