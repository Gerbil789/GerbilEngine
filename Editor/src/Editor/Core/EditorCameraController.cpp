#include "EditorCameraController.h"
#include "Engine/Core/Input.h"

namespace Editor
{
	using namespace Engine;

	void EditorCameraController::SetViewportSize(const glm::vec2& size)
	{
		m_Camera.SetViewportSize(size); // Update aspect ratio
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
		glm::vec3 position = m_Camera.GetPosition();

		switch (e.GetKey())
		{
		case Key::W: position += m_Camera.GetForward() * m_MoveSpeed; break;									// Move forward (camera forward)
		case Key::S: position -= m_Camera.GetForward() * m_MoveSpeed; break;									// Move backward (camera backward)
		case Key::A: position -= m_Camera.GetRight() * m_MoveSpeed; break;										// Move left (camera left)
		case Key::D: position += m_Camera.GetRight() * m_MoveSpeed; break;										// Move right (camera right)
		case Key::LeftShift: position -= glm::vec3(0.0f, 1.0f, 0.0f) * m_MoveSpeed; break;		// Move down (world down)
		case Key::Space: position += glm::vec3(0.0f, 1.0f, 0.0f) * m_MoveSpeed; break;				// Move up (world up)
		default: return false; 
		}

		m_Camera.SetPosition(position);
		LOG_INFO("Camera Position: {0}, {1}, {2}", position.x, position.y, position.z);
		return false;
	}

	bool EditorCameraController::OnKeyReleased(KeyReleasedEvent& e)
	{
		return false;
	}

	bool EditorCameraController::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * m_ScrollSensitivity;
		glm::vec3 position = m_Camera.GetPosition();
		m_Camera.SetPosition(position + m_Camera.GetForward() * delta * m_ScrollSensitivity);
		return false;
	}

	bool EditorCameraController::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if(e.GetMouseButton() == Mouse::ButtonRight)
		{
			m_RotateDragging = true;
			m_StartMousePosition = Input::GetMousePosition();
		}
		else if (e.GetMouseButton() == Mouse::ButtonMiddle)
		{
			m_PanDragging = true;
			m_StartMousePosition = Input::GetMousePosition();
		}
		return false;
	}

	bool EditorCameraController::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonRight)
		{
			m_RotateDragging = false;
		}
		else if (e.GetMouseButton() == Mouse::ButtonMiddle)
		{
			m_PanDragging = false;
		}
		return false;
	}

	bool EditorCameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		if (!m_RotateDragging && !m_PanDragging) return false;

		glm::vec2 mouse = { e.GetX(), e.GetY() };
		glm::vec2 delta = (mouse - m_StartMousePosition) * m_MouseSensitivity;
		m_StartMousePosition = mouse;

		if (m_RotateDragging)
		{
			float yaw = m_Camera.GetYaw() - delta.x;
			float pitch = m_Camera.GetPitch() - delta.y;
			m_Camera.SetRotation(pitch, yaw);

		}
		else if (m_PanDragging)
		{
			glm::vec3 position = m_Camera.GetPosition();
			glm::vec3 right = m_Camera.GetRight();
			glm::vec3 up = m_Camera.GetUp();
			position -= right * delta.x * m_PanSpeed;
			position += up * delta.y * m_PanSpeed;
			m_Camera.SetPosition(position);
		}
		return false;
	}

}


