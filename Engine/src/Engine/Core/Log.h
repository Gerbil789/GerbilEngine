#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/UUID.h"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <filesystem>
#include <string>
#include <webgpu/webgpu.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

//TODO: replace fmt with std::format ?

namespace Engine
{
	class Log
	{
	public:
		static void Initialize();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
	};
}

template <>
struct fmt::formatter<std::filesystem::path> : fmt::formatter<std::string> {
	template <typename FormatContext>
	auto format(const std::filesystem::path& path, FormatContext& ctx) {
		return fmt::formatter<std::string>::format(path.string(), ctx);
	}
};

template <>
struct fmt::formatter<WGPUStringView> : fmt::formatter<std::string> {
	template <typename FormatContext>
	auto format(const WGPUStringView& strView, FormatContext& ctx) {
		std::string s = (strView.data && strView.length > 0) ?
			std::string(strView.data, strView.length) :
			std::string{};
		return fmt::formatter<std::string>::format(s, ctx);
	}
};

template <>
struct fmt::formatter<Engine::UUID> : fmt::formatter<uint64_t>
{
	template <typename FormatContext>
	auto format(const Engine::UUID& uuid, FormatContext& ctx)
	{
		return fmt::formatter<uint64_t>::format(static_cast<uint64_t>(uuid), ctx);
	}
};

#define LOG_TRACE(...)		::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)			::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_WARNING(...)  ::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)		::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)
