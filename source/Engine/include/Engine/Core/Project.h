#pragma once

#include <filesystem>

namespace engine
{
  struct Project
  {
    static void Initialize(const std::filesystem::path& projectDirectory)
    {
      directory = projectDirectory;
      assetsDirectory = projectDirectory / "Assets";
    }

    inline static std::filesystem::path directory;
    inline static std::filesystem::path assetsDirectory;
  };
}