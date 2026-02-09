#pragma once

#include "Engine/Core/API.h"
#include <filesystem>
#include <functional>

namespace Engine
{
	class Scene;

	using SceneChangedCallback = std::function<void(Scene* newScene)>;

	namespace SceneManager
	{
		ENGINE_API void RegisterOnSceneChanged(const SceneChangedCallback& callback);
		ENGINE_API void NotifySceneChanged();

		ENGINE_API void SetActiveScene(Scene* scene);
		ENGINE_API Scene* GetActiveScene();

		ENGINE_API void CreateScene(const std::filesystem::path& path);
		ENGINE_API void LoadScene();
		ENGINE_API void LoadScene(const std::filesystem::path& path);
		ENGINE_API void SaveScene();
		ENGINE_API void SaveSceneAs();
	}
}

