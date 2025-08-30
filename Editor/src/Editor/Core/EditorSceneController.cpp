#include "EditorSceneController.h"
#include "Engine/Core/Input.h"
#include "Engine/Scene/Entity.h"

namespace Editor::EditorSceneController
{
	using namespace Engine;

	//Engine::UUID m_CopiedEntityUUID = 0;
	entt::entity m_SelectedEntity = entt::null;

	void OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(OnKeyPressed);
	}

	bool OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKey())
		{
		case Key::N:
		{
			if (control)
				SceneManager::CreateScene("NewScene");
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



	template<typename... Components>
	void CopyComponents(entt::registry& registry, entt::entity src, entt::entity dst) {
		([&] {
			if (registry.any_of<Components>(src)) {
				const auto& component = registry.get<Components>(src);
				registry.emplace_or_replace<Components>(dst, component);
			}
			}(), ...);
	}


	void DuplicateEntity(Entity entity)
	{
		if (!entity) { return; }

		Entity newEntity = SceneManager::GetActiveScene()->CreateEntity(entity.GetName());

		CopyComponents<TransformComponent, MeshComponent, LightComponent, NameComponent, HierarchyComponent>(
			SceneManager::GetActiveScene()->m_Registry, entity, newEntity
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

	void SelectEntity(Entity entity)
	{
		m_SelectedEntity = entity;
	}

	void DeselectEntity()
	{
		m_SelectedEntity = entt::null;
	}

	bool IsEntitySelected(Entity entity)
	{
		return m_SelectedEntity == entity;
	}

	Entity GetSelectedEntity()
	{
		return { m_SelectedEntity, &SceneManager::GetActiveScene()->m_Registry };
	}
}