#pragma once

#include "EditorWindow.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	class SceneHierarchyWindow : public EditorWindow, public ISceneObserver
	{
	public:
		SceneHierarchyWindow(EditorContext* context);
		~SceneHierarchyWindow();

		void OnSceneChanged(Ref<Scene> newScene) override;
		void OnImGuiRender() override;

	private:
		Ref<Scene> m_Scene;

		void DrawEntityNode(Entity entity);
	};
}