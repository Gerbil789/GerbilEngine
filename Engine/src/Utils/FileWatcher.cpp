#include "enginepch.h"
#include "Engine/Utils/FileWatcher.h"

namespace Engine
{
	FileWatcher::FileWatcher(const std::filesystem::path& directory) : m_Directory(directory)
	{
		OnUpdate(); // Initial scan to populate existing files
	}

	void FileWatcher::OnUpdate()
	{
		//TODO: handle file moves/renames

		for (auto& entry : std::filesystem::recursive_directory_iterator(m_Directory))
		{
			if (!entry.is_regular_file())
			{
				continue; // skip directories, symlinks, etc.
			}

			auto lastWrite = std::filesystem::last_write_time(entry);
			auto it = m_LastWriteTimes.find(entry.path());

			if (it == m_LastWriteTimes.end())
			{
				// New file
				m_LastWriteTimes[entry.path()] = lastWrite;
				if (m_EventCallback)
				{
					FileAddedEvent e(entry.path());
					m_EventCallback(e);
				}
			}
			else if (it->second != lastWrite)
			{
				// Modified file
				it->second = lastWrite;
				if (m_EventCallback)
				{
					FileModifiedEvent e(entry.path());
					m_EventCallback(e);
				}
			}
		}

		// check for deleted files
		for (auto it = m_LastWriteTimes.begin(); it != m_LastWriteTimes.end(); )
		{
			if (!std::filesystem::exists(it->first))
			{
				if (m_EventCallback)
				{
					FileRemovedEvent e(it->first);
					m_EventCallback(e);
				}
				it = m_LastWriteTimes.erase(it);
			}
			else
				++it;
		}
	}
}


