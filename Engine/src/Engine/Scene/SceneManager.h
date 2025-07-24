#pragma once

#include <filesystem>

namespace Engine
{
	class Scene;
}

namespace Engine::SceneManager
{
	void SetActiveScene(Scene* scene);
	Scene* GetActiveScene();

	void CreateScene(const std::filesystem::path& path);
	void LoadScene();
	void LoadScene(const std::filesystem::path& path);
	void SaveScene();
	void SaveSceneAs();
}