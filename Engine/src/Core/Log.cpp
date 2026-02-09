#include "enginepch.h"
#include "Engine/Core/Log.h"
#include <print>
#include <mutex>

#if defined(ENGINE_PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace Engine
{
  static std::string MakeTimestamp()
  {
    using clock = std::chrono::system_clock;
    auto now = clock::now();
    auto t = clock::to_time_t(now);

    std::tm tm{};
#if defined(ENGINE_PLATFORM_WINDOWS)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buffer;
  }

  namespace
  {
    std::ofstream s_LogFile;

    void OpenLogFile()
    {
      std::error_code ec;
      std::filesystem::create_directories("logs", ec);

      auto now = std::chrono::system_clock::now();
      auto t = std::chrono::system_clock::to_time_t(now);

      std::tm tm{};
#if defined(ENGINE_PLATFORM_WINDOWS)
      localtime_s(&tm, &t);
#else
      localtime_r(&t, &tm);
#endif

      char name[64];
      std::snprintf(name, sizeof(name), "logs/%02d-%02d-%04d_%02d-%02d-%02d.log", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
      s_LogFile.open(name, std::ios::out | std::ios::app);
    }
  }

  namespace
  {
    std::mutex s_LogMutex;

    enum class Level
    {
      Trace,
			Debug,
      Info,
      Warn,
      Error,
      Critical
    };

    constexpr std::string_view Color(Level level)
    {
			// ANSI escape codes for colors
      switch (level)
      {
      case Level::Trace:     return "\033[37m";        // white
      case Level::Debug:     return "\033[36m";        // cyan
      case Level::Info:      return "\033[32m";        // green
      case Level::Warn:      return "\033[33m\033[1m"; // yellow + bold
      case Level::Error:     return "\033[31m\033[1m"; // red + bold
      case Level::Critical:  return "\033[1m\033[41m"; // bold + red background
      }
      return "\033[m"; // reset
    }

    constexpr std::string_view ResetColor()
    {
      return "\033[0m";
    }

    void Print(Level level, std::string_view message)
    {
      std::scoped_lock lock(s_LogMutex);
      std::string timestamp = MakeTimestamp();

      // Console
      std::println("{}[{}] {}{}", Color(level), timestamp, message, ResetColor());

      // File
      if (s_LogFile.is_open())
      {
        s_LogFile << '[' << timestamp << "] " << message << '\n';
        s_LogFile.flush();
      }
    }
  }

  void Log::Initialize()
  {
#if defined(ENGINE_PLATFORM_WINDOWS)
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle != INVALID_HANDLE_VALUE)
    {
      DWORD mode = 0;
      if (GetConsoleMode(handle, &mode))
      {
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(handle, mode);
      }
    }
#endif
    OpenLogFile();
  }

  void Log::Trace(std::string_view msg) { Print(Level::Trace, msg); }
  void Log::Info(std::string_view msg) { Print(Level::Info, msg); }
  void Log::Warn(std::string_view msg) { Print(Level::Warn, msg); }
  void Log::Error(std::string_view msg) { Print(Level::Error, msg); }
  void Log::Critical(std::string_view msg) { Print(Level::Critical, msg); }
}