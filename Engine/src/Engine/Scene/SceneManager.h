#pragma once

#include <filesystem>
#include <functional>

namespace Engine
{
	class Scene;

	using SceneChangedCallback = std::function<void(Scene* newScene)>;

	namespace SceneManager
	{
		void RegisterOnSceneChanged(const SceneChangedCallback& callback);
		void NotifySceneChanged();

		void SetActiveScene(Scene* scene);
		Scene* GetActiveScene();

		void CreateScene(const std::filesystem::path& path);
		void LoadScene();
		void LoadScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();
	}
}

