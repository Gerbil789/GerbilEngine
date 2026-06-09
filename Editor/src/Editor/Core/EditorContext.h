#pragma once

#include "Engine/Graphics/Camera.h"
#include <filesystem>
#include <glm/glm.hpp>


namespace Editor
{
	enum class EditorState { Edit, Play };
	enum class PlayMode { PIE, Windowed, Fullscreen }; // PIE - Play In Editor

	struct EditorContext
	{
		static EditorState state;
		static PlayMode playMode;
		static Engine::Camera editorCamera;
	};

	namespace EditorSettings //TODP: convert to struct and move to its own file
	{
		inline std::filesystem::path projectDirectory = std::filesystem::path();
		inline bool showGrid = true;
		inline glm::vec4 wireframeColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

		void Load();
		void Save();
	};
}