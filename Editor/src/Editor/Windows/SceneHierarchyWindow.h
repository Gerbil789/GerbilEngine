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
	class SceneHierarchyWindow : public EditorWindow, public Engine::ISceneObserver
	{
	public:
		SceneHierarchyWindow(EditorContext* context);
		~SceneHierarchyWindow();

		void OnSceneChanged(Ref<Engine::Scene> newScene) override;
		void OnImGuiRender() override;

	private:
		Ref<Engine::Scene> m_Scene;

		void DrawEntityNode(entt::entity);
		void DrawReorderDropTarget(entt::entity parent, size_t index);

		SceneController* m_SceneController;
	};
}