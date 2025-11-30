#pragma once

#include "IEditorWindow.h"
#include "Engine/Scene/Entity.h"

namespace Editor
{
	class SceneHierarchyWindow : public IEditorWindow
	{
	public:
		SceneHierarchyWindow();
		void OnUpdate() override;

	private:
		Engine::Scene* m_Scene = nullptr;

		void DrawEntityNode(Engine::Entity);
		void DrawReorderDropTarget(Engine::Entity parent, size_t index);
	};
}