#pragma once

#include "Engine/Event/FileEvent.h"
#include <thread>
#include <atomic>

namespace Editor
{
  class FileWatcher
  {
  public:
    FileWatcher(const std::filesystem::path& directory, const std::function<void(std::unique_ptr<Engine::FileEvent>)>& callback);
    ~FileWatcher();

  private:
		void Watch();

  private:
    std::filesystem::path m_Directory;
    std::function<void(std::unique_ptr<Engine::FileEvent>)> m_EventCallback;

    std::thread m_WatcherThread;
    std::atomic<bool> m_Running = false;
  };
}