#pragma once

#include <filesystem>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

inline std::filesystem::path GetExecutableDir() {
#ifdef _WIN32
  char buffer[MAX_PATH];
  GetModuleFileNameA(NULL, buffer, MAX_PATH);
  return std::filesystem::path(buffer).parent_path();
#else
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  return std::filesystem::path(std::string(result, count)).parent_path();
#endif
}