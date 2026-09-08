#pragma once

#include "Engine/Graphics/Renderer/Renderer.h"
#include "Editor/Core/EditorSettings.h"
#include "Engine/Core/Components.h"

namespace editor
{
	enum class EditorMode { Edit, Play };
	enum class PlayMode { PIE, Windowed, Fullscreen }; // PIE - Play In Editor

	struct EditorContext
	{
		EditorMode editorMode = EditorMode::Edit;
		PlayMode playMode = PlayMode::PIE;

		engine::CameraComponent camera;
		engine::TransformComponent cameraTransform;

		EditorSettings settings;
	};

	inline EditorContext editorContext;
}