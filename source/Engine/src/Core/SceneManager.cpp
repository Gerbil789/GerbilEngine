#include "enginepch.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/ApplicationEvent.h"
#include "Engine/Core/Log.h"

namespace engine
{
	void SceneManager::SetActiveScene(Scene scene)
	{
		s_ActiveScene = scene;
		engine::EventBus::Publish(SceneChangedEvent{ s_ActiveScene });

		LOG_INFO("Active scene set to '{}' ({})", AssetRegistry::GetAssetPath(s_ActiveScene).filename(), s_ActiveScene.id);
	}

	Scene SceneManager::GetActiveScene()
	{
		return s_ActiveScene;
	}
}
