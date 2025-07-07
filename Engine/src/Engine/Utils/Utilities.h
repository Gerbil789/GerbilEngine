#pragma once
#include <string>
#include <filesystem>

namespace Engine
{
	class Utilities
	{
	public:
		static std::string EnsureFileNameUniqueness(const std::string& filename, const std::string& path); //TODO: delete this, its ugly
	};
}