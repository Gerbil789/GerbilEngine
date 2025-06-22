#pragma once

#include "Engine/Events/Event.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	class SceneController : public ISceneObserver
	{
	public:
		SceneController();
		~SceneController();

		void OnSceneChanged(Ref<Scene> newScene) override;
		void OnEvent(Event& e);

		// Entity management functions
		void DuplicateEntity(Entity entity);
		//void CopyEntity(Entity entity);
		//void PasteEntity();
		void SelectEntity(Entity entity);
		void DeselectEntity();
		bool IsEntitySelected(Entity entity) const;
		Entity GetSelectedEntity();


	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		void OnScenePlay();
		void OnSceneStop();

		Ref<Scene> m_Scene;
		UUID m_CopiedEntityUUID = 0;
		entt::entity m_SelectedEntity = entt::null;
	};
}