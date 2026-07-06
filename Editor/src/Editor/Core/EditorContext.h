#pragma once

#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Camera.h"
#include "Editor/Core/EditorSettings.h"

namespace Editor
{
	enum class EditorMode { Edit, Play };
	enum class PlayMode { PIE, Windowed, Fullscreen }; // PIE - Play In Editor

	struct EditorContext
	{
		EditorMode editorMode = EditorMode::Edit;
		PlayMode playMode = PlayMode::PIE;

		Engine::Renderer renderer;
		Engine::Camera editorCamera;

		EditorSettings settings;
	};

	inline EditorContext editorContext;
}