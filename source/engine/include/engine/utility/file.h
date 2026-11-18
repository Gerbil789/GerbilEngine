#pragma once

#include <filesystem>

namespace engine 
{
	bool ReadFile(const std::filesystem::path& path, std::string& outData);
}