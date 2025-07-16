#pragma once

#include "EditorWindow.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Events/MouseEvent.h"
#include "Editor/Services/SceneController.h"
#include "Editor/Core/Core.h"
#include "Editor/Core/EditorCameraController.h"
#include <glm/glm.hpp>

namespace Editor
{
	class ViewportWindow : public EditorWindow
	{
	public:
		ViewportWindow(EditorWindowManager* context);

		void OnUpdate(Engine::Timestep ts) override;
		void OnEvent(Engine::Event& e) override;

	private:
		bool OnKeyPressed(Engine::KeyPressedEvent& e);
		bool OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e);
		void UpdateViewportSize();

	private:
		Engine::Scene* m_Scene;
		Engine::Renderer m_Renderer;
		EditorCameraController m_CameraController;
		SceneController* m_SceneController;

		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		int m_GizmoType = 7; //translate

		Engine::Entity m_HoveredEntity;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
	};
}