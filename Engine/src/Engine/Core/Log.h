#pragma once

#include "Engine/Core/UUID.h"
#include <filesystem>
#include <webgpu/webgpu.h>
#include <format>

namespace Engine
{
	class Log
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void Trace(const std::string& message);
		static void Info(const std::string& message);
		static void Warn(const std::string& message);
		static void Error(const std::string& message);
		static void Critical(const std::string& message);
	};
}

template <>
struct std::formatter<std::filesystem::path, char> : std::formatter<std::string_view, char>
{
	auto format(const std::filesystem::path& p, format_context& ctx) const
	{
		return std::formatter<std::string_view, char>::format(
			p.string(), ctx
		);
	}
};

template <>
struct std::formatter<WGPUStringView, char>
	: std::formatter<std::string_view, char>
{
	auto format(const WGPUStringView& strView, format_context& ctx) const
	{
		std::string_view sv =
			(strView.data && strView.length > 0)
			? std::string_view(strView.data, strView.length)
			: std::string_view{};

		return std::formatter<std::string_view, char>::format(sv, ctx);
	}
};

template <>
struct std::formatter<Engine::UUID, char>
	: std::formatter<uint64_t, char>
{
	auto format(const Engine::UUID& uuid, format_context& ctx) const
	{
		return std::formatter<uint64_t, char>::format(
			static_cast<uint64_t>(uuid),
			ctx
		);
	}
};

#define LOG_TRACE(...)		::Engine::Log::Trace(std::format(__VA_ARGS__))
#define LOG_INFO(...)			::Engine::Log::Info(std::format(__VA_ARGS__))
#define LOG_WARNING(...)  ::Engine::Log::Warn(std::format(__VA_ARGS__))
#define LOG_ERROR(...)		::Engine::Log::Error(std::format(__VA_ARGS__))
#define LOG_CRITICAL(...) ::Engine::Log::Critical(std::format(__VA_ARGS__))


