#include "enginepch.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Core/Log.h"

namespace Engine::SceneManager
{
	static Uuid m_ActiveScene;

	void SetActiveScene(Uuid id)
	{
		if(!id)
		{
			LOG_ERROR("Invalid scene ID provided to SetActiveScene");
			return;
		}

		m_ActiveScene = id;
		LOG_INFO("Active scene set to {}", m_ActiveScene);
	}

	Uuid GetActiveScene()
	{
		return m_ActiveScene;
	}
}
