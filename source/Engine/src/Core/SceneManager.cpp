#include "enginepch.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Asset/AssetRegistry.h"

namespace engine
{
	void SceneManager::SetActiveScene(SceneHandle scene)
	{
		s_activeScene = scene;
		LOG_INFO("Active scene set to '{}' ({})", AssetRegistry::GetAssetPath(s_activeScene).filename(), s_activeScene.id);
	}

	SceneHandle SceneManager::GetActiveScene()
	{
		return s_activeScene;
	}
}
