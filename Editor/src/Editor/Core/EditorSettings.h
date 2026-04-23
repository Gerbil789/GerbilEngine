#pragma once

#include <filesystem>
#include <glm/glm.hpp>

namespace Editor
{
	struct EditorSettings
	{
		std::filesystem::path projectDirectory = std::filesystem::path();

		bool showGrid = true;

		glm::vec4 wireframeColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	};

	extern EditorSettings g_EditorSettings;

	void LoadEditorSettings();
	void SaveEditorSettings();
}