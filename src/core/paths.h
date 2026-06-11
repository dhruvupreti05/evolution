#pragma once

#include <filesystem>
#include <string>

class Paths
{
public:
    static std::filesystem::path getRepoRoot();
    static std::filesystem::path getAssetPath(const std::string& relativePath);
};