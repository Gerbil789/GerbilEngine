#pragma once

#include "Engine/Events/Event.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Core/Core.h"

namespace Editor
{
	class SceneController : public Engine::ISceneObserver
	{
	public:
		SceneController();
		~SceneController();

		void OnSceneChanged(Ref<Engine::Scene> newScene) override;
		void OnEvent(Engine::Event& e);

		// Entity management functions
		void DuplicateEntity(Engine::Entity entity);
		//void CopyEntity(Entity entity);
		//void PasteEntity();
		void SelectEntity(Engine::Entity entity);
		void DeselectEntity();
		bool IsEntitySelected(Engine::Entity entity) const;
		Engine::Entity GetSelectedEntity();


	private:
		bool OnKeyPressed(Engine::KeyPressedEvent& e);

		void OnScenePlay();
		void OnSceneStop();

		Ref<Engine::Scene> m_Scene;
		Engine::UUID m_CopiedEntityUUID = 0;
		entt::entity m_SelectedEntity = entt::null;
	};
}