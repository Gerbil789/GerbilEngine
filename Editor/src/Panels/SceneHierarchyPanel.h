#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();
	private:
		Ref<Scene> m_Context;

		void DrawEntityNode(Entity entity);

		Entity m_SelectionContext;
	};
	
}