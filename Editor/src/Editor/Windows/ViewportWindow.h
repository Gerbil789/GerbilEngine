#pragma once

#include <glm/glm.hpp>
#include "EditorWindow.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/Renderer.h"
#include "Editor/Services/SceneController.h"
#include "Editor/Core/Core.h"

namespace Editor
{
	class ViewportWindow : public EditorWindow, public Engine::ISceneObserver
	{
	public:
		ViewportWindow(EditorWindowManager* context);
		~ViewportWindow();

		void OnSceneChanged(Ref<Engine::Scene> newScene) override;
		void OnUpdate(Engine::Timestep ts) override;
		void OnEvent(Engine::Event& e) override;

	private:
		bool OnKeyPressed(Engine::KeyPressedEvent& e);
		bool OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e);

	private:
		Ref<Engine::Scene> m_Scene;
		Engine::Renderer m_Renderer;
		SceneController* m_SceneController;

		Ref<Engine::EditorCamera> m_EditorCamera; //TODO: move editor camera to Editor project ?

		int m_GizmoType = 7; //translate

		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		Engine::Entity m_HoveredEntity;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
	};
}