#include "enginepch.h"
#include "Log.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Engine
{
	static std::shared_ptr<spdlog::logger> s_CoreLogger;

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
		localtime_r(&t, &tm); // Linux
#endif

		std::ostringstream oss;
		oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");

		std::filesystem::path logFile = logDir / (oss.str() + ".log");

		std::array<spdlog::sink_ptr, 2> logSinks;

		// console log
		logSinks[0] = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		logSinks[0]->set_pattern("%^[%T]: %v%$");	

		// file log
		logSinks[1] = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile.string(), true);
		logSinks[1]->set_pattern("[%T] [%l]: %v");


		s_CoreLogger = std::make_shared<spdlog::logger>("ENGINE", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);
	}

	void Log::Shutdown()
	{

	}

	void Log::Trace(const std::string& msg) { s_CoreLogger->trace(msg); }
	void Log::Info(const std::string& msg) { s_CoreLogger->info(msg); }
	void Log::Warn(const std::string& msg) { s_CoreLogger->warn(msg); }
	void Log::Error(const std::string& msg) { s_CoreLogger->error(msg); }
	void Log::Critical(const std::string& msg) { s_CoreLogger->critical(msg); }
}