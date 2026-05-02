#pragma once

#include "IEditorWindow.h"
#include "Engine/Scene/Scene.h"

namespace Editor
{
	class SceneHierarchyWindow : public IEditorWindow
	{
	public:
		void Draw() override;

	private:
		void DrawEntityNode(entt::registry& registry, entt::entity entity);
		void DrawReorderDropTarget(entt::registry& registry, entt::entity parent, size_t index);
	};
}