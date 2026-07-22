#pragma once

#include "Engine/Event/Event.h"
#include <filesystem>

namespace Engine
{
  struct FileAddedEvent : public Event
	{
    std::filesystem::path path;

    FileAddedEvent(const std::filesystem::path& path) : path(path) {}
	};

  struct FileRemovedEvent : public Event
  {
    std::filesystem::path path;

    FileRemovedEvent(const std::filesystem::path& path) : path(path) {}
	};

  struct FileModifiedEvent : public Event
  {
    std::filesystem::path path;

    FileModifiedEvent(const std::filesystem::path& path) : path(path) {}
  };
}