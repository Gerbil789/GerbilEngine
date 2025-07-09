#pragma once
#include <string>
#include <filesystem>
#include <optional>

namespace Engine 
{
	std::optional<std::string> ReadFile(const std::filesystem::path& path); //TODO: investigate if there is a better way to read files

}