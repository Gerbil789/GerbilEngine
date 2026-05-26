#pragma once

#include "Engine/Scene/Scene.h"

namespace Engine::SceneManager
{
	ENGINE_API void SetActiveScene(Uuid id);
	ENGINE_API Scene& GetActiveScene();

	ENGINE_API void SaveScene(const std::filesystem::path& path);
	ENGINE_API void SaveSceneAs();
}