#pragma once

#include "Engine/Event/FileEvent.h"

namespace Engine
{
  class FileWatcher
  {
  public:
    using EventCallbackFn = std::function<void(Event&)>;

    FileWatcher(const std::filesystem::path& directory);
		~FileWatcher() = default;

		void OnUpdate();
    void SetEventCallback(const EventCallbackFn& callback) { m_EventCallback = callback; }

  private:
    std::filesystem::path m_Directory;
    EventCallbackFn m_EventCallback;

    std::unordered_map<std::filesystem::path, std::filesystem::file_time_type> m_LastWriteTimes;
  };
}