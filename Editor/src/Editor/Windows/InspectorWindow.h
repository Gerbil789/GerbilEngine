#pragma once

#include "EditorWindow.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Services/SceneController.h"

namespace Editor
{
	class InspectorWindow : public EditorWindow
	{
	public:
		InspectorWindow(EditorWindowManager* context);

		void OnUpdate(Engine::Timestep ts) override;

	private:
		Engine::Scene* m_Scene;

		void DrawComponents(Engine::Entity entity);
		void DrawAddComponentButton(Engine::Entity entity);

		SceneController* m_SceneController;
	};
}