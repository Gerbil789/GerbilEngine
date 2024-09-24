#pragma once
#include <string>
#include <filesystem>
#include <optional>

namespace Engine 
{
	std::optional<std::string> ReadFile(const std::filesystem::path& path);

}