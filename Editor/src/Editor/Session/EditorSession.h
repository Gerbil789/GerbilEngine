#pragma once

#include "Engine/Scene/SceneManager.h"
#include "Editor/Command/CommandManager.h"

namespace Editor
{
	class EditorSession
	{
	public:
		EditorSession()
		{
			Engine::SceneManager::RegisterOnSceneChanged([this](Engine::Scene* scene) { m_Scene = scene; OnSceneChanged(); });
			m_Scene = Engine::SceneManager::GetActiveScene();
		}

		virtual ~EditorSession() = default;

		CommandManager& GetCommandManager() { return m_CommandManager; }

	protected:
		virtual void OnSceneChanged() {}

	protected:
		Engine::Scene* m_Scene = nullptr;
		CommandManager m_CommandManager;
	};
}