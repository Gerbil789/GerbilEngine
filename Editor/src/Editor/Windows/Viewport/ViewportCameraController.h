#pragma once

#include "Engine/Graphics/Camera.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/KeyEvent.h"

namespace Editor
{
	class ViewportCameraController
	{
	public:
		ViewportCameraController(Engine::Camera* camera);

	private:
		bool OnKeyPressed(Engine::KeyPressedEvent& e);
		bool OnMouseScroll(Engine::MouseScrolledEvent& e);
		bool OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(Engine::MouseButtonReleasedEvent& e);
		bool OnMouseMoved(Engine::MouseMovedEvent& e);

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

	ViewportCameraController& GetCameraController();
	void SetCameraController(ViewportCameraController* controller);

}