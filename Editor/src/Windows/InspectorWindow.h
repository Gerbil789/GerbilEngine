#pragma once

#include "EditorWindow.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	class InspectorWindow : public EditorWindow, public ISceneObserver
	{
	public:
		InspectorWindow(EditorContext* context);
		~InspectorWindow();

		void OnSceneChanged(Ref<Scene> newScene) override;
		void OnImGuiRender() override;

	private:
		Ref<Scene> m_Scene;

		void DrawComponents(Entity entity);
		void DrawAddComponentButton(Entity entity);
	};
}