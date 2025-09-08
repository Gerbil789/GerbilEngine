#include "EditorCameraController.h"
#include "Engine/Core/Input.h"
#include "Engine/Scene/Entity.h"
#include "Editor/Core/EditorSceneController.h"

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
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressed));
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OnMouseScroll));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(OnMouseButtonReleased));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(OnMouseMoved));
	}

	bool EditorCameraController::OnKeyPressed(KeyPressedEvent& e)
	{
		if(e.GetKey() == Key::F) //Focus
		{
			auto entity = EditorSceneController::GetSelectedEntity();
			if (!entity) return false;

			glm::vec3 focusPoint = entity.GetComponent<TransformComponent>().Position;
			FocusOnPoint(focusPoint);
		}

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

	void EditorCameraController::FocusOnPoint(const glm::vec3& point, float distance)
	{
		glm::vec3 position = point - m_Camera.GetForward() * distance;
		m_Camera.SetPosition(position);
	}

}


