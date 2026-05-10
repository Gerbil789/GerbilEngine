#pragma once

#include <filesystem>

namespace Editor
{
	std::string OpenFile();
	std::string SaveFile();

	std::filesystem::path OpenDirectory();

	void OpenFileExplorer(const std::filesystem::path& path);
}