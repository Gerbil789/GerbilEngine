#pragma once

#include "EditorWindow.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Services/SceneController.h"
#include "Editor/Core/Core.h"

namespace Editor
{
	class SceneHierarchyWindow : public EditorWindow
	{
	public:
		SceneHierarchyWindow(EditorWindowManager* context);

		void OnUpdate(Engine::Timestep ts) override;

	private:
		Engine::Scene* m_Scene;
		SceneController* m_SceneController;

		void DrawEntityNode(entt::entity);
		void DrawReorderDropTarget(entt::entity parent, size_t index);
	};
}