#pragma once

#include "Engine/Scene/Scene.h"

namespace Engine::SceneManager
{
	ENGINE_API void SetActiveScene(Scene& scene);
	ENGINE_API Scene& GetActiveScene();

	ENGINE_API void SaveScene();
	ENGINE_API void SaveSceneAs();
}