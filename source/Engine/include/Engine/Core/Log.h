#pragma once

#include "Engine/Core/UUID.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <format>
#include <source_location>

namespace engine
{
	class Log
	{
	public:
		static void Initialize();

		static void Trace(std::string_view message, const std::source_location& location = std::source_location::current());
		static void Info(std::string_view message, const std::source_location& location = std::source_location::current());
		static void Warn(std::string_view message, const std::source_location& location = std::source_location::current());
		static void Error(std::string_view message, const std::source_location& location = std::source_location::current());
		static void Critical(std::string_view message, const std::source_location& location = std::source_location::current());
	};
}

template <>
struct std::formatter<std::filesystem::path, char> : std::formatter<std::string_view, char>
{
	auto format(const std::filesystem::path& p, auto& ctx) const
	{
		return std::formatter<std::string_view, char>::format(p.string(), ctx);
	}
};

template <>
struct std::formatter<engine::Uuid, char> : std::formatter<uint64_t, char>
{
	auto format(const engine::Uuid& uuid, auto& ctx) const
	{
		return std::formatter<uint64_t, char>::format(static_cast<uint64_t>(uuid), ctx);
	}
};

#define LOG_TRACE(...)		engine::Log::Trace(std::format(__VA_ARGS__))
#define LOG_INFO(...)			engine::Log::Info(std::format(__VA_ARGS__))
#define LOG_WARNING(...)  engine::Log::Warn(std::format(__VA_ARGS__))
#define LOG_ERROR(...)		engine::Log::Error(std::format(__VA_ARGS__))
#define LOG_CRITICAL(...) engine::Log::Critical(std::format(__VA_ARGS__))