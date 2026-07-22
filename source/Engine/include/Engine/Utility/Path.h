#pragma once

#include <filesystem>

#ifdef ENGINE_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

inline std::filesystem::path GetExecutableDir() 
{
#ifdef ENGINE_PLATFORM_WINDOWS
  char buffer[MAX_PATH];
  GetModuleFileNameA(NULL, buffer, MAX_PATH);
  return std::filesystem::path(buffer).parent_path();
#else
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  return std::filesystem::path(std::string(result, count)).parent_path();
#endif
}

inline void SetupWorkingDirectory()
{
  std::filesystem::path exeDir = GetExecutableDir();

  if (std::filesystem::exists(exeDir / "Resources")) 
  {
    std::filesystem::current_path(exeDir);
    return;
  }

  std::filesystem::path searchPath = exeDir;
  while (searchPath.has_parent_path()) 
  {
    if (std::filesystem::exists(searchPath / "Resources")) 
    {
      std::filesystem::current_path(searchPath);
      return;
    }
    searchPath = searchPath.parent_path();
  }

  throw std::runtime_error("Resources folder not found. Please ensure the working directory is set correctly.");
}