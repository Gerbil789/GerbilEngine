#pragma once

#include <filesystem>

namespace Engine 
{
	bool ReadFile(const std::filesystem::path& path, std::string& outData);
}