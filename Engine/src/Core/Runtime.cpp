#include "enginepch.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	void Runtime::Start()
	{
		if (m_GameInstance)
		{
			LOG_WARNING("Runtime already started");
			return;
		}

		m_GameInstance = std::make_unique<GameInstance>();

		m_GameInstance->OnExit = []() { m_GameInstance.reset(); };
		Engine::Scene* activeScene = Engine::SceneManager::GetActiveScene();
		m_GameInstance->Initialize(activeScene);
	}

	void Runtime::Stop()
	{
		if (!m_GameInstance)
		{
			LOG_WARNING("Runtime not running");
			return;
		}

		m_GameInstance->Close();
		m_GameInstance.reset();
	}

	void Runtime::Update(float delta)
	{
		if (m_GameInstance)
		{
			m_GameInstance->Update(delta);
		}
	}
}