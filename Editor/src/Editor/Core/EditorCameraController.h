#pragma once

#include "Engine/Renderer/Camera.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"

namespace Editor
{
	class EditorCameraController
	{
	public:
		EditorCameraController() = default;

		void SetViewportSize(glm::vec2 size);
		void OnEvent(Engine::Event& e);

		Engine::Camera& GetCamera() { return m_Camera; }

	private:
		bool OnKeyPressed(Engine::KeyPressedEvent& e);
		bool OnKeyReleased(Engine::KeyReleasedEvent& e);
		bool OnMouseScroll(Engine::MouseScrolledEvent& e);
		bool OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(Engine::MouseButtonReleasedEvent& e);
		bool OnMouseMoved(Engine::MouseMovedEvent& e);

	private:
		Engine::Camera m_Camera;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_StartMousePosition = { 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = glm::vec3(0.0f);

		bool m_Dragging = false;

		float m_MouseSensitivity = 0.01f;
		float m_ScrollSensitivity = 0.01f;
	};
}