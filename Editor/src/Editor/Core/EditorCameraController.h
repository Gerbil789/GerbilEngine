#pragma once

#include "Engine/Renderer/Camera.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/KeyEvent.h"

namespace Editor
{
	class EditorCameraController
	{
	public:
		void SetViewportSize(const glm::vec2& size);
		void OnEvent(Engine::Event& e);

		Engine::Camera& GetCamera() { return m_Camera; }

	private:
		bool OnKeyPressed(Engine::KeyPressedEvent& e);
		bool OnMouseScroll(Engine::MouseScrolledEvent& e);
		bool OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(Engine::MouseButtonReleasedEvent& e);
		bool OnMouseMoved(Engine::MouseMovedEvent& e);

		void FocusOnPoint(const glm::vec3& point, float distance = 10.0f);

	private:
		Engine::Camera m_Camera;

		glm::vec2 m_StartMousePosition = { 0.0f, 0.0f };
		bool m_RotateDragging = false;
		bool m_PanDragging = false;

		//TODO: Expose these in editor settings
		float m_MouseSensitivity = 0.08f;
		float m_ScrollSensitivity = 1.0f;
		float m_PanSpeed = 0.1f;
	};
}