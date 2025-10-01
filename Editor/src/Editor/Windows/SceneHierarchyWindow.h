#pragma once

#include "EditorWindow.h"
#include "Engine/Scene/Entity.h"

namespace Editor
{
	class SceneHierarchyWindow : public EditorWindow
	{
	public:
		SceneHierarchyWindow();
		void OnUpdate(Engine::Timestep ts) override;

	private:
		Engine::Scene* m_Scene = nullptr;

		void DrawEntityNode(Engine::Entity);
		void DrawReorderDropTarget(Engine::Entity parent, size_t index);
	};
}