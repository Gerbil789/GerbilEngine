#pragma once

inline std::filesystem::path EDITOR_RESOURCES; // Path to the Editor resources directory


inline std::filesystem::path GetExecutableDir() {
#ifdef _WIN32
  char buffer[MAX_PATH];
  GetModuleFileNameA(NULL, buffer, MAX_PATH);
  return std::filesystem::path(buffer).parent_path();
#else
  // For Linux/macOS if needed
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  return std::filesystem::path(std::string(result, count)).parent_path();
#endif
}

namespace Editor
{
  namespace Icon
  {
    inline const auto Checker = EDITOR_RESOURCES / "textures/checker.png";
    inline const auto Grid = EDITOR_RESOURCES / "textures/grid.png";
  }
}