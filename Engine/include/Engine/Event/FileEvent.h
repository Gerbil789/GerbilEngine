#pragma once

#include "Engine/Event/Event.h"

namespace Engine
{
	class FileEvent : public Event
  {
  public:
    std::filesystem::path GetPath() const { return path; }

		virtual EventType GetEventType() const { return EventType::None; }
		virtual const char* GetName() const { return "FileEvent"; }


    EVENT_CLASS_CATEGORY(EventCategoryFile)
  protected:
		FileEvent(const std::filesystem::path& path) : path(path) {}
    std::filesystem::path path;
  };

	class FileAddedEvent : public FileEvent
	{
    public:
    FileAddedEvent(const std::filesystem::path& path) : FileEvent(path) {}
    std::string ToString() const override
    {
      std::stringstream ss;
      ss << "FileAddedEvent: " << path;
      return ss.str();
    }
    EVENT_CLASS_TYPE(FileAdded)
	};

  class FileRemovedEvent : public FileEvent
  {
  public:
    FileRemovedEvent(const std::filesystem::path& path) : FileEvent(path) {}
    std::string ToString() const override
    {
      std::stringstream ss;
      ss << "FileRemovedEvent: " << path;
      return ss.str();
    }
    EVENT_CLASS_TYPE(FileRemoved)
	};

  class FileModifiedEvent : public FileEvent
  {
  public:
    FileModifiedEvent(const std::filesystem::path& path) : FileEvent(path) {}
    std::string ToString() const override
    {
      std::stringstream ss;
      ss << "FileModifiedEvent: " << path;
      return ss.str();
    }
    EVENT_CLASS_TYPE(FileModified)
  };

}