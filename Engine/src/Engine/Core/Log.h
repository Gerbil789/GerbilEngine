#pragma once

#include "Engine/Core/Core.h"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include <filesystem>
#include <string>
#include <webgpu/webgpu.h>
// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#pragma warning(pop)

namespace Engine
{
	class Log
	{
	public:
		static void Init();
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


#define LOG_TRACE(...)		::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)			::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_WARNING(...)  ::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)		::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)
