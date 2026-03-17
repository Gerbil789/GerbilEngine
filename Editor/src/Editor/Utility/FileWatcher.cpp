#include "Editor/Utility/FileWatcher.h"
#include "Engine/Core/Log.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/FileEvent.h"
#include <thread>
#include <atomic>
#include <windows.h>

namespace Editor
{
  HANDLE m_DirHandle = INVALID_HANDLE_VALUE;
  std::thread m_Thread;
  std::atomic<bool> m_Running = false;

  void Watch(const std::filesystem::path& directory)
  {
    m_DirHandle = CreateFileW(
      directory.wstring().c_str(),
      FILE_LIST_DIRECTORY,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      nullptr,
      OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS,
      nullptr
    );

    if (m_DirHandle == INVALID_HANDLE_VALUE)
    {
      LOG_ERROR("Failed to watch directory: {}", directory);
      return;
    }

    constexpr DWORD bufferSize = 16 * 1024;
    std::vector<BYTE> buffer(bufferSize);

    while (m_Running)
    {
      DWORD bytesReturned = 0;

      bool ok = ReadDirectoryChangesW(
        m_DirHandle,
        buffer.data(),
        bufferSize,
        TRUE, // recursive
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
        &bytesReturned,
        nullptr,
        nullptr);

      if (!ok || !m_Running)
      {
        break;
      }

      BYTE* ptr = buffer.data();

      while (true)
      {
        FILE_NOTIFY_INFORMATION* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(ptr);

        std::wstring fileName(info->FileName, info->FileNameLength / sizeof(WCHAR));

        std::filesystem::path fullPath = directory / fileName;

        switch (info->Action)
        {
        case FILE_ACTION_ADDED:
        {
          auto e = std::make_unique<Engine::FileAddedEvent>(fullPath);
          Engine::EventBus::Get().Publish(*e);
          break;
        }
        case FILE_ACTION_REMOVED:
        {
          auto e = std::make_unique<Engine::FileRemovedEvent>(fullPath);
          Engine::EventBus::Get().Publish(*e);
          break;
        }
        case FILE_ACTION_MODIFIED:
        {
          auto e = std::make_unique<Engine::FileModifiedEvent>(fullPath);
          Engine::EventBus::Get().Publish(*e);
          break;
        }
        /* case FILE_ACTION_RENAMED_OLD_NAME:
         {

         }
         case FILE_ACTION_RENAMED_NEW_NAME:
         {

         }*/
        }

        if (info->NextEntryOffset == 0)
        {
          break;
        }

        ptr += info->NextEntryOffset;
      }
    }

    CloseHandle(m_DirHandle);
    m_DirHandle = INVALID_HANDLE_VALUE;
  }

	void FileWatcher::WatchDirectory(const std::filesystem::path& directory)
	{
		m_Running = true;
    m_Thread = std::thread(&Watch, directory);
	}

  void FileWatcher::Shutdown()
  {
    m_Running = false;

    if (m_DirHandle != INVALID_HANDLE_VALUE)
    {
      CancelIoEx(m_DirHandle, nullptr);
    }

    if (m_Thread.joinable())
    {
      m_Thread.join();
    }
  }


	
}