#pragma once

#include "Engine/Graphics/Renderer/Renderer.h"
#include "Editor/Core/EditorSettings.h"
#include "Engine/Core/Components.h"

namespace Editor
{
	enum class EditorMode { Edit, Play };
	enum class PlayMode { PIE, Windowed, Fullscreen }; // PIE - Play In Editor

	struct EditorContext
	{
		EditorMode editorMode = EditorMode::Edit;
		PlayMode playMode = PlayMode::PIE;

		Engine::Renderer renderer;
		Engine::CameraComponent camera;
		Engine::TransformComponent cameraTransform;

		EditorSettings settings;
	};

	inline EditorContext editorContext;
}