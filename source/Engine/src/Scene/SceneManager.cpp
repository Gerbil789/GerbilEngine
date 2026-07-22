#include "enginepch.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/ApplicationEvent.h"
#include "Engine/Core/Log.h"

namespace Engine::SceneManager
{
	static Uuid m_ActiveScene;

	void SetActiveScene(Uuid id)
	{
		m_ActiveScene = id;
		Engine::EventBus::Publish(SceneChangedEvent{ m_ActiveScene });

		LOG_INFO("Active scene set to {}", m_ActiveScene);
	}

	Uuid GetActiveScene()
	{
		return m_ActiveScene;
	}
}
