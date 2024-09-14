#include "enginepch.h"
#include "Utilities.h"

namespace Engine
{
	std::string Utilities::EnsureFileNameUniqueness(const std::string& filename, const std::string& path)
	{
		std::string filepath = path + "/" + filename;

		if (!std::filesystem::exists(filepath))
		{
			return filepath;
		}

		filepath += "_";
		int i = 1;
		while (std::filesystem::exists(filepath + std::to_string(i)))
		{
			i++;
		}
		return filepath + std::to_string(i);
	}
}

