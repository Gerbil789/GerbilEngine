#pragma once

#include <string>
#include <filesystem>
#include <optional>

namespace Engine 
{
	bool ReadFile(const std::filesystem::path& path, std::string& outData);

	std::string OpenFile(const char* filter); 
	std::string SaveFile(const char* filter); 
}