#include "SceneController.h"
#include "Engine/Core/Input.h"
#include "Engine/Scene/Entity.h"

namespace Editor
{
	using namespace Engine;

	SceneController::SceneController()
	{
		SceneManager::RegisterObserver(this);
	}

	SceneController::~SceneController()
	{
		SceneManager::UnregisterObserver(this);
	}

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
				DuplicateEntity(GetSelectedEntity());
			}
			break;
		}
		case Key::C:
		{
			if (control)
			{
				//CopyEntity(GetSelectedEntity());
			}
			break;
		}
		case Key::V:
		{
			if (control)
			{
				//PasteEntity();
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

	template<typename... Components>
	void CopyComponents(entt::registry& registry, entt::entity src, entt::entity dst) {
		([&] {
			if (registry.any_of<Components>(src)) {
				const auto& component = registry.get<Components>(src);
				registry.emplace_or_replace<Components>(dst, component);
			}
			}(), ...);
	}


	void SceneController::DuplicateEntity(Entity entity)
	{
		if (!entity) { return; }

		Entity newEntity = m_Scene->CreateEntity(entity.GetName());

		CopyComponents<TransformComponent, SpriteComponent, MeshComponent, LightComponent, NameComponent, HierarchyComponent>(
			m_Scene->m_Registry, entity, newEntity
		);

		SelectEntity(newEntity);
	}

	/*void SceneController::CopyEntity(Entity entity)
	{
		m_CopiedEntityUUID = entity.GetUUID();
	}

	void SceneController::PasteEntity()
	{
		if (m_CopiedEntityUUID == 0) { return; }

		auto view = m_Scene->m_Registry.view<IDComponent>();
		for (auto entity : view)
		{
			if (view.get<IDComponent>(entity).ID == m_CopiedEntityUUID)
			{
				DuplicateEntity(Entity{ entity, this });
				return;
			}
		}
	}*/

	void SceneController::SelectEntity(Entity entity)
	{
		m_SelectedEntity = entity;
	}

	void SceneController::DeselectEntity()
	{
		m_SelectedEntity = entt::null;
	}

	bool SceneController::IsEntitySelected(Entity entity) const
	{
		return m_SelectedEntity == entity;
	}

	Entity SceneController::GetSelectedEntity()
	{
		return { m_SelectedEntity, &m_Scene->m_Registry };
	}
}