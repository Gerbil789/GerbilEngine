#pragma once

#include "Engine/Events/Event.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	class SceneController : public ISceneObserver
	{
	public:
		SceneController() = default;

		void OnSceneChanged(Ref<Scene> newScene) override;
		void OnEvent(Event& e);

	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		void OnScenePlay();
		void OnSceneStop();

		Ref<Scene> m_Scene;
	};
}