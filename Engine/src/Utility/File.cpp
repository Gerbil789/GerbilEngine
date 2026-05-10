#include "enginepch.h"
#include "Engine/Utility/File.h"

namespace Engine
{
	bool ReadFile(const std::filesystem::path& path, std::string& outData)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (!in)
		{
			LOG_ERROR("Could not open file '{}'", path);
			return false;
		}

		outData.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		return true;
	}
}