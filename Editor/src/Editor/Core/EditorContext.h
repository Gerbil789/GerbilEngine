#pragma once

#include <filesystem>
#include <glm/glm.hpp>

namespace Editor
{
	enum class EditorState { Edit, Play };
	enum class PlayMode { PIE, Windowed, Fullscreen }; // PIE - Play In Editor

	namespace EditorContext
	{
		inline EditorState state = EditorState::Edit;
		inline PlayMode playMode = PlayMode::PIE;
	}

	namespace EditorSettings
	{
		inline std::filesystem::path projectDirectory = std::filesystem::path();
		inline bool showGrid = true;
		inline glm::vec4 wireframeColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

		void Load();
		void Save();
	};
}