#include "SceneController.h"
#include "Engine/Core/Input.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
	void SceneController::OnSceneChanged(Ref<Scene> newScene)
	{
		m_Scene = newScene;
	}

	void SceneController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ENGINE_BIND_EVENT_FN(SceneController::OnKeyPressed));
	}


	bool SceneController::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
				SceneManager::CreateScene("New Scene");
			break;
		}
		case Key::O:
		{
			if (control)
			{
				SceneManager::LoadScene();
			}
			break;
		}
		case Key::S:
		{
			if (control)
			{
				if (shift)
				{
					SceneManager::SaveSceneAs();
				}
				else
				{
					SceneManager::SaveScene();
				}
			}
			break;
		}
		case Key::D:
		{
			if (control)
			{
				auto selectedEntity = SceneManager::GetCurrentScene()->GetSelectedEntity();
				if (selectedEntity)
				{
					SceneManager::GetCurrentScene()->DuplicateEntity(selectedEntity);
				}
			}
			break;
		}
		case Key::C:
		{
			if (control)
			{
				m_Scene->CopyEntity(m_Scene->GetSelectedEntity());
			}
			break;
		}
		case Key::V:
		{
			if (control)
			{
				m_Scene->PasteEntity();
			}
			break;
		}
		}
	}


	void SceneController::OnScenePlay()
	{
		/*m_SceneState = Scene::SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_InspectorPanel.SetContext(m_ActiveScene);*/
	}

	void SceneController::OnSceneStop()
	{
		/* m_ActiveScene->OnRuntimeStop();
		 m_SceneState = Scene::SceneState::Edit;
		 m_ActiveScene = m_EditorScene;
		 m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		 m_InspectorPanel.SetContext(m_ActiveScene);*/
	}
}