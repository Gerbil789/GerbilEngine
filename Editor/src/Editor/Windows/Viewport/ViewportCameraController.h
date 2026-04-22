#pragma once

#include "Engine/Graphics/Camera.h"

namespace Engine
{
	class KeyPressedEvent;
	class MouseScrolledEvent;
	class MouseButtonPressedEvent;
	class MouseButtonReleasedEvent;
	class MouseMovedEvent;
}

namespace Editor
{
	class ViewportCameraController
	{
	public:
		void Initialize(Engine::Camera* camera);

	private:
		void OnKeyPressed(Engine::KeyPressedEvent& e);
		void OnMouseScroll(Engine::MouseScrolledEvent& e);
		void OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e);
		void OnMouseButtonReleased(Engine::MouseButtonReleasedEvent& e);
		void OnMouseMoved(Engine::MouseMovedEvent& e);

		void FocusOnPoint(const glm::vec3& point, float distance = 10.0f);

	private:
		Engine::Camera* m_Camera = nullptr;

		glm::vec2 m_StartMousePosition = { 0.0f, 0.0f };
		bool m_RotateDragging = false;
		bool m_PanDragging = false;

		float m_MouseSensitivity = 0.12f;
		float m_ScrollSensitivity = 1.0f;
		float m_PanSpeed = 0.1f;
	};
}