#include "enginepch.h"
#include "File.h"
#include <portable-file-dialogs.h>

namespace Engine
{
	std::optional<std::string> ReadFile(const std::filesystem::path& path)
	{
		ENGINE_PROFILE_FUNCTION();

		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (!in)
		{
			LOG_ERROR("Could not open file '{0}'", path);
			return std::optional<std::string>();
		}

		std::string result((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		return result;
	}

	std::string OpenFile(const char* filter)
	{
		auto result = pfd::open_file("Open File", ".", { "All Files", "*" }).result();
		return !result.empty() ? result[0] : "";
	}

	std::string SaveFile(const char* filter)
	{
		auto result = pfd::save_file("Save File", ".", { "All Files", "*" }).result();
		return result;
	}
}