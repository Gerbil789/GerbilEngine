#pragma once

#include "EditorWindowBase.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	class InspectorWindow : public EditorWindowBase, public ISceneObserver
	{
	public:
		InspectorWindow();
		~InspectorWindow();

		void OnSceneChanged() override;
		void OnImGuiRender() override;

	private:
		Ref<Scene> m_Scene;

		void DrawComponents(Entity entity);
		void DrawAddComponentButton(Entity entity);
	};
}