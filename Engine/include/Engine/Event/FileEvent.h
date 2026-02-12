#pragma once

#include "Engine/Event/Event.h"

namespace Engine
{
	class FileEvent : public Event
  {
  public:
    std::filesystem::path GetPath() const { return path; }

    EVENT_CLASS_CATEGORY(EventCategoryFile)

		FileEvent(const std::filesystem::path& path) : path(path) {}
    std::filesystem::path path;
  };

	class FileAddedEvent : public FileEvent
	{
    public:
    FileAddedEvent(const std::filesystem::path& path) : FileEvent(path) {}
    EVENT_CLASS_TYPE(FileAdded)
	};

  class FileRemovedEvent : public FileEvent
  {
  public:
    FileRemovedEvent(const std::filesystem::path& path) : FileEvent(path) {}
    EVENT_CLASS_TYPE(FileRemoved)
	};

  class FileModifiedEvent : public FileEvent
  {
  public:
    FileModifiedEvent(const std::filesystem::path& path) : FileEvent(path) {}
    EVENT_CLASS_TYPE(FileModified)
  };

}