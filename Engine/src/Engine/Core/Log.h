#pragma once

#include "Engine/Core/Core.h"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include <filesystem>
#include <string>

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
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

template <>
struct fmt::formatter<std::filesystem::path> : fmt::formatter<std::string> {
	template <typename FormatContext>
	auto format(const std::filesystem::path& path, FormatContext& ctx) {
		return fmt::formatter<std::string>::format(path.string(), ctx);
	}
};

#define ENGINE_LOG_TRACE(...) ::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENGINE_LOG_INFO(...)  ::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENGINE_LOG_WARNING(...)  ::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_LOG_ERROR(...) ::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENGINE_LOG_CRITICAL(...) ::Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define LOG_TRACE(...) ::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)  ::Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARNING(...)  ::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Engine::Log::GetClientLogger()->critical(__VA_ARGS__)
