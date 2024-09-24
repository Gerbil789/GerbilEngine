#include "enginepch.h"
#include "Engine/Utils/File.h"

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
}