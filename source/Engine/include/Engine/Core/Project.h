#pragma once

#include <filesystem>

namespace engine
{
  class Project
  {
  public:
    static void Initialize(const std::filesystem::path& projectDirectory)
    {
      s_Directory = projectDirectory;
      s_AssetsDirectory = projectDirectory / "Assets";
    }
    static const std::filesystem::path &Directory() { return s_Directory; }
    static const std::filesystem::path &AssetsDirectory() { return s_AssetsDirectory; }

  private:
    inline static std::filesystem::path s_Directory;
    inline static std::filesystem::path s_AssetsDirectory;
  };
}