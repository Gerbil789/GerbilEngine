#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	class SceneHierarchyPanel : public ISceneObserver
	{
	public:
		SceneHierarchyPanel();
		~SceneHierarchyPanel();

		void OnSceneChanged() override;
		void OnImGuiRender();

	private:
		Ref<Scene> m_Scene;

		void DrawEntityNode(Entity entity);
	};
}