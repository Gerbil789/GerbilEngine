#include "enginepch.h"
#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Engine
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;

	void Log::Initialize()
	{
		std::filesystem::path logDir = std::filesystem::current_path() / "logs";
		if (!std::filesystem::exists(logDir))
		{
			std::filesystem::create_directories(logDir);
		}

		// --- Create timestamped filename ---
		auto now = std::chrono::system_clock::now();
		auto t = std::chrono::system_clock::to_time_t(now);

		std::tm tm{};
#if defined(_WIN32)
		localtime_s(&tm, &t); // Windows
#else
		localtime_r(&t, &tm); // Linux / Mac
#endif

		std::ostringstream oss;
		oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");

		std::filesystem::path logFile = logDir / (oss.str() + ".log");

		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile.string(), true));

		logSinks[0]->set_pattern("%^[%T]: %v%$");		// console pattern
		logSinks[1]->set_pattern("[%T] [%l]: %v");	// file pattern

		s_CoreLogger = std::make_shared<spdlog::logger>("ENGINE", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);
	}
}