#pragma once

#include <filesystem>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

//TODO: fix this file

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



inline std::filesystem::path GetSettingsFilePath(const std::string& appName) 
{
  std::filesystem::path configDir;

#if defined(_WIN32)
  char* appData = nullptr;
  size_t len = 0;

  if (_dupenv_s(&appData, &len, "APPDATA") == 0 && appData)
  {
    configDir = std::filesystem::path(appData) / appName;
    free(appData);
  }
#elif defined(__APPLE__)
  // macOS: ~/Library/Application Support/MyAwesomeEditor
  const char* home = std::getenv("HOME");
  if (home) {
    configDir = std::filesystem::path(home) / "Library" / "Application Support" / appName;
  }
#else 
  // Linux/Unix: ~/.config/MyAwesomeEditor (XDG Base Directory Spec)
  const char* xdgConfig = std::getenv("XDG_CONFIG_HOME");
  if (xdgConfig && std::string(xdgConfig) != "") {
    configDir = std::filesystem::path(xdgConfig) / appName;
  }
  else {
    const char* home = std::getenv("HOME");
    if (home) {
      configDir = std::filesystem::path(home) / ".config" / appName;
    }
  }
#endif

  // Fallback just in case environment variables fail
  if (configDir.empty()) {
    configDir = std::filesystem::current_path() / appName;
  }

  // Ensure the folder actually exists before we try to save a file inside it
  if (!std::filesystem::exists(configDir)) {
    std::filesystem::create_directories(configDir);
  }

  // Return the full path including the file name
  return configDir / "editor_settings.yaml";
}