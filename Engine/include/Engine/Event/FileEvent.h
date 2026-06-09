#pragma once

#include "Engine/Event/Event.h"
#include <filesystem>

namespace Engine
{
  struct FileEvent : public Event
  {
    std::filesystem::path path;

    FileEvent(const std::filesystem::path& path) : path(path) {}
  };

  struct FileAddedEvent : public FileEvent
	{
    FileAddedEvent(const std::filesystem::path& path) : FileEvent(path) {}
	};

  struct FileRemovedEvent : public FileEvent
  {
    FileRemovedEvent(const std::filesystem::path& path) : FileEvent(path) {}
	};

  struct FileModifiedEvent : public FileEvent
  {
    FileModifiedEvent(const std::filesystem::path& path) : FileEvent(path) {}
  };
}