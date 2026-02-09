#pragma once

#include "Engine/Core/API.h"
#include "Engine/Core/UUID.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <format>

namespace Engine
{
	class ENGINE_API Log
	{
	public:
		static void Initialize();

		static void Trace(std::string_view message);
		static void Info(std::string_view message);
		static void Warn(std::string_view message);
		static void Error(std::string_view message);
		static void Critical(std::string_view message);
	};
}

template <>
struct std::formatter<std::filesystem::path, char>
	: std::formatter<std::string_view, char>
{
	auto format(const std::filesystem::path& p, format_context& ctx) const
	{
		return std::formatter<std::string_view, char>::format(p.string(), ctx);
	}
};

template <>
struct std::formatter<Engine::Uuid, char>
	: std::formatter<uint64_t, char>
{
	auto format(const Engine::Uuid& uuid, format_context& ctx) const
	{
		return std::formatter<uint64_t, char>::format(
			static_cast<uint64_t>(uuid), ctx);
	}
};

#define LOG_TRACE(...)		Engine::Log::Trace(std::format(__VA_ARGS__))
#define LOG_INFO(...)			Engine::Log::Info(std::format(__VA_ARGS__))
#define LOG_WARNING(...)  Engine::Log::Warn(std::format(__VA_ARGS__))
#define LOG_ERROR(...)		Engine::Log::Error(std::format(__VA_ARGS__))
#define LOG_CRITICAL(...) Engine::Log::Critical(std::format(__VA_ARGS__))