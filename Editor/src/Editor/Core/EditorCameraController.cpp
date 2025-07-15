#include "EditorCameraController.h"
#include "Engine/Core/Input.h"

namespace Editor
{
	using namespace Engine;

	void EditorCameraController::SetViewportSize(glm::vec2 size)
	{
		m_ViewportSize = size;
		m_Camera.SetViewportSize(size);
	}

	void EditorCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ENGINE_BIND_EVENT_FN(OnKeyPressed));
		dispatcher.Dispatch<KeyReleasedEvent>(ENGINE_BIND_EVENT_FN(OnKeyReleased));
		dispatcher.Dispatch<MouseScrolledEvent>(ENGINE_BIND_EVENT_FN(OnMouseScroll));
		dispatcher.Dispatch<MouseButtonPressedEvent>(ENGINE_BIND_EVENT_FN(OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(ENGINE_BIND_EVENT_FN(OnMouseButtonReleased));
		dispatcher.Dispatch<MouseMovedEvent>(ENGINE_BIND_EVENT_FN(OnMouseMoved));
	}

	bool EditorCameraController::OnKeyPressed(KeyPressedEvent& e)
	{
		if(e.GetKeyCode() == Key::LeftShift) // Move down
		{
			m_Camera.SetPosition(m_Camera.GetPosition() + glm::vec3(0.0f, -0.1f, 0.0f));
			m_Camera.UpdateViewMatrix();
		}

		if (e.GetKeyCode() == Key::LeftControl) // Move up
		{
			m_Camera.SetPosition(m_Camera.GetPosition() + glm::vec3(0.0f, 0.1f, 0.0f));
			m_Camera.UpdateViewMatrix();
		}

		if (e.GetKeyCode() == Key::A) // Move left
		{
			m_Camera.SetPosition(m_Camera.GetPosition() + glm::vec3(-0.1f, 0.0f, 0.0f));
			m_Camera.UpdateViewMatrix();
		}

		if (e.GetKeyCode() == Key::D) // Move right
		{
			m_Camera.SetPosition(m_Camera.GetPosition() + glm::vec3(0.1f, 0.0f, 0.0f));
			m_Camera.UpdateViewMatrix();
		}

		if (e.GetKeyCode() == Key::W) // Move forward
		{
			m_Camera.SetPosition(m_Camera.GetPosition() + glm::vec3(0.0f, 0.0f, 0.1f));
			m_Camera.UpdateViewMatrix();
		}

		if (e.GetKeyCode() == Key::S) // Move backward
		{
			m_Camera.SetPosition(m_Camera.GetPosition() + glm::vec3(0.0f, 0.0f, -0.1f));
			m_Camera.UpdateViewMatrix();
		}

		if (e.GetKeyCode() == Key::Q) // Zoom in
		{
			m_Camera.SetZoom(m_Camera.GetZoom() + 0.1f);
			m_Camera.UpdateViewMatrix();
			LOG_INFO("Zoom: {0}", m_Camera.GetZoom());
		}

		if (e.GetKeyCode() == Key::E) // Zoom out
		{
			m_Camera.SetZoom(m_Camera.GetZoom() - 0.1f);
			m_Camera.UpdateViewMatrix();
			LOG_INFO("Zoom: {0}", m_Camera.GetZoom());
		}

		return false;
	}

	bool EditorCameraController::OnKeyReleased(KeyReleasedEvent& e)
	{
		return false;
	}

	bool EditorCameraController::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * m_ScrollSensitivity;
		float zoom = m_Camera.GetZoom() + delta;
		zoom = glm::clamp(zoom, -2.0f, 2.0f);
		m_Camera.SetZoom(zoom);
		m_Camera.UpdateViewMatrix();

		return false;
	}

	bool EditorCameraController::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if(e.GetMouseButton() == Mouse::ButtonRight)
		{
			m_Dragging = true;
			m_StartMousePosition = Input::GetMousePosition();
		}
		return false;
	}

	bool EditorCameraController::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonRight)
		{
			m_Dragging = false;
		}
		return false;
	}

	bool EditorCameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		if (!m_Dragging) return false;

		glm::vec2 mouse = { e.GetX(), e.GetY() };
		glm::vec2 delta = (mouse - m_StartMousePosition) * m_MouseSensitivity;
		m_StartMousePosition = mouse;

		float yaw = m_Camera.GetYaw() + delta.x;
		float pitch = m_Camera.GetPitch() - delta.y;
		m_Camera.SetRotation(pitch, yaw);
		m_Camera.UpdateViewMatrix();
		return false;
	}

}


