#pragma once

#include <filesystem>
#include <string>

/*
    Central place for building file paths used by the project.
*/
class Paths
{
public:
    static std::filesystem::path getRepoRoot();
    static std::filesystem::path getAssetPath(const std::string& relativePath);
};