#pragma once

#include <filesystem>

namespace Editor
{
  class FileWatcher
  {
  public:
    static void WatchDirectory(const std::filesystem::path& directory);
		static void Shutdown();
  };
}