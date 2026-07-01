#include "enginepch.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/ApplicationEvent.h"
#include "Engine/Core/Log.h"

namespace Engine::SceneManager
{
	static Uuid m_ActiveScene;

	void SetActiveScene(Uuid id)
	{
		if (!AssetManager::Exists(id))
		{
			LOG_ERROR("Scene with ID {} does not exist", id);
			return;
		}

		m_ActiveScene = id;
		Engine::EventBus::Publish(SceneChangedEvent{ m_ActiveScene });

		LOG_INFO("Active scene set to {}", m_ActiveScene);
	}

	Uuid GetActiveScene()
	{
		return m_ActiveScene;
	}
}
