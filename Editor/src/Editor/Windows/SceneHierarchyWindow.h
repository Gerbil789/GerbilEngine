#pragma once

#include "IEditorWindow.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"

namespace Editor
{
	class SceneHierarchyWindow : public IEditorWindow
	{
	public:
		void Initialize() override;
		void Draw() override;

	private:
		Engine::Scene* m_Scene = nullptr;

		void DrawEntityNode(Engine::Entity);
		void DrawReorderDropTarget(Engine::Entity parent, size_t index);
	};
}