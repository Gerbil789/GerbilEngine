#pragma once

#include <filesystem>

namespace EditorSettings
{
	inline std::filesystem::path projectDirectory = std::filesystem::path();

	void Load();
	void Save();
}