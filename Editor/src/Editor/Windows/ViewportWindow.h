#pragma once

#include "EditorWindow.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/EntityIdRenderer.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Event/MouseEvent.h"
#include "Editor/Core/EditorCameraController.h"
#include <glm/glm.hpp>

namespace Editor
{
	class ViewportWindow : public EditorWindow
	{
	public:
		ViewportWindow();
		void OnUpdate(Engine::Timestep ts) override;
		void OnEvent(Engine::Event& e) override;

	private:
		bool OnKeyPressed(Engine::KeyPressedEvent& e);
		bool OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e);
		void UpdateViewportSize();

		void DrawGizmos();

	private:
		Engine::Scene* m_Scene = nullptr;
		Engine::Renderer m_Renderer = Engine::Renderer(640, 640);
		Engine::EntityIdRenderer m_EntityIdRenderer = Engine::EntityIdRenderer(640, 640);
		EditorCameraController m_CameraController;


		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		int m_GizmoType = 7; //translate

		Engine::Entity m_HoveredEntity;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
	};
}