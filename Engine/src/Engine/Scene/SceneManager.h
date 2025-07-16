#pragma once

#include "Engine/Scene/Scene.h"

namespace Engine::SceneManager
{
	extern Ref<Scene> s_ActiveSceneRef; // Keeps it alive
	extern Scene* s_ActiveScene;        // Fast access

	void SetActiveScene(const Ref<Scene>& scene);
	inline Scene* GetActiveScene() { return s_ActiveScene; }

	void CreateScene(const std::filesystem::path& path);
	void LoadScene();
	void LoadScene(const std::filesystem::path& path);
	void SaveScene();
	void SaveSceneAs();
}