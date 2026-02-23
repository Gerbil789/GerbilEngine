#pragma once

#include "Engine/Core/API.h"
#include <functional>

namespace Engine
{
	class Scene;
	class ScriptRegistry;

	namespace SceneManager
	{
		ENGINE_API void Initialize(ScriptRegistry& registry);

		ENGINE_API void RegisterOnSceneChanged(const std::function<void(Scene*)>& callback);
		ENGINE_API void NotifySceneChanged();

		ENGINE_API void SetActiveScene(Scene* scene);
		ENGINE_API Scene* GetActiveScene();

		ENGINE_API void SaveScene();
		ENGINE_API void SaveSceneAs();
	}
}

