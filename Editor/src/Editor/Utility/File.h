#pragma once

#include <string>
#include <filesystem>
#include <initializer_list>

namespace Editor::FileDialog
{
	struct DialogFilter
	{
		std::string name; // e.g., "Scene Files"
		std::string spec; // e.g., "*.scene"
	};

	std::string SelectFile(std::initializer_list<DialogFilter> filters = {});
	std::string SelectPath(std::initializer_list<DialogFilter> filters = {}, const char* defaultExt = nullptr);
	std::filesystem::path SelectDirectory();
	void OpenFileExplorer(const std::filesystem::path& path);
}