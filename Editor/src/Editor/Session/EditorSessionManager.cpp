#include "enginepch.h"
#include "EditorSessionManager.h"
#include "Engine/Core/Input.h"

namespace Editor
{
	void EditorSessionManager::OnEvent(Engine::Event& e)
	{
		Engine::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Engine::KeyPressedEvent>([this](auto e) {OnKeyPressed(e); });
	}

	void EditorSessionManager::OnKeyPressed(Engine::KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0) return;

		if (!Engine::Input::IsKeyPressed(Engine::Key::LeftControl))
		{
			return;
		}

		if(e.GetKey() == Engine::Key::Y)
		{
			if (Engine::Input::IsKeyPressed(Engine::Key::LeftShift))
			{
				if (m_SceneSession)
				{
					LOG_INFO("Redo");
					m_SceneSession->GetCommandManager().Redo();
				}

			}
			else
			{
				if (m_SceneSession)
				{
					LOG_INFO("Undo");
					m_SceneSession->GetCommandManager().Undo();
				}

			}
		}
	}



	SceneEditorSession* EditorSessionManager::GetSceneSession()
	{
		if (!m_SceneSession)
		{
			m_SceneSession = new SceneEditorSession();
		}
		return m_SceneSession;
	}
}