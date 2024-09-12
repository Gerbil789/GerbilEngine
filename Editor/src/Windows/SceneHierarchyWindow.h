#pragma once

#include "EditorWindowBase.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	class SceneHierarchyWindow : public EditorWindowBase, public ISceneObserver
	{
	public:
		SceneHierarchyWindow();
		~SceneHierarchyWindow();

		void OnSceneChanged() override;
		void OnImGuiRender() override;

	private:
		Ref<Scene> m_Scene;

		void DrawEntityNode(Entity entity);
	};
}