#pragma once

#include "Editor/Core/EditorSettings.h"
#include "Engine/Asset/AssetType.h"
#include "Engine/Core/Components.h"
#include <entt/entity/entity.hpp>

namespace editor
{
	enum class EditorMode { Edit, Play };
	enum class PlayMode { PIE, Windowed, Fullscreen }; // PIE - Play In Editor
}

namespace editor::context
{
	inline EditorMode editorMode = EditorMode::Edit;
	inline PlayMode playMode = PlayMode::PIE;

	//TODO: move camera into viewport window?
	inline engine::CameraComponent camera;
	inline engine::TransformComponent cameraTransform;

	inline EditorSettings settings;

	struct Asset {
		engine::Uuid id = 0;
		engine::AssetType type = engine::AssetType::Unknown;
	};

	inline Asset selectedAsset;

	inline entt::entity selectedEntity = entt::null;
}