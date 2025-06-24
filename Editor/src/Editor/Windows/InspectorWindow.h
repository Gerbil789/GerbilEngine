#pragma once

#include "EditorWindow.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Services/SceneController.h"

namespace Editor
{
	class InspectorWindow : public EditorWindow, public Engine::ISceneObserver
	{
	public:
		InspectorWindow(EditorContext* context);
		~InspectorWindow();

		void OnSceneChanged(Ref<Engine::Scene> newScene) override;
		void OnImGuiRender() override;

	private:
		Ref<Engine::Scene> m_Scene;

		void DrawComponents(Engine::Entity entity);
		void DrawAddComponentButton(Engine::Entity entity);

		SceneController* m_SceneController;
	};
}