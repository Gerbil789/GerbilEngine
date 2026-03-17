#include "EditorCameraController.h"
#include "Engine/Core/Input.h"
#include "Engine/Scene/Entity.h"
#include "Editor/Core/EditorSelection.h"
#include "Engine/Scene/Components.h"
#include "Engine/Event/EventBus.h"

namespace Editor
{
	namespace
	{
		EditorCameraController* s_Controller = nullptr;
	}

	EditorCameraController& GetCameraController() {
		return *s_Controller;
	}

	void SetCameraController(EditorCameraController* controller) {
		s_Controller = controller;
	}

	EditorCameraController::EditorCameraController(Engine::Camera* camera) : m_Camera(camera)
	{
		Engine::EventBus::Get().Subscribe<Engine::KeyPressedEvent>([this](auto e) {OnKeyPressed(e); });
		Engine::EventBus::Get().Subscribe<Engine::MouseScrolledEvent>([this](auto e) {OnMouseScroll(e); });
		Engine::EventBus::Get().Subscribe<Engine::MouseButtonPressedEvent>([this](auto e) {OnMouseButtonPressed(e); });
		Engine::EventBus::Get().Subscribe<Engine::MouseButtonReleasedEvent>([this](auto e) {OnMouseButtonReleased(e); });
		Engine::EventBus::Get().Subscribe<Engine::MouseMovedEvent>([this](auto e) {OnMouseMoved(e); });
	}

	bool EditorCameraController::OnKeyPressed(Engine::KeyPressedEvent& e)
	{
		if (e.GetKey() == Engine::KeyCode::F) //Focus
		{
			if (EditorSelection::Entities().Empty())
			{
				return false;
			}

			auto entity = EditorSelection::Entities().GetPrimary();
			if (!entity) return false; //TODO: must return bool?

			glm::vec3 focusPoint = entity.Get<Engine::TransformComponent>().position;
			FocusOnPoint(focusPoint);
		}

		return false;
	}



	bool EditorCameraController::OnMouseScroll(Engine::MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * m_ScrollSensitivity;
		glm::vec3 position = m_Camera->GetPosition();
		m_Camera->SetPosition(position + m_Camera->GetForward() * delta * m_ScrollSensitivity);
		return false;
	}

	bool EditorCameraController::OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Engine::MouseCode::ButtonRight)
		{
			m_RotateDragging = true;
			m_StartMousePosition = Engine::Input::GetMousePosition();
		}
		else if (e.GetMouseButton() == Engine::MouseCode::ButtonMiddle)
		{
			m_PanDragging = true;
			m_StartMousePosition = Engine::Input::GetMousePosition();
		}
		return false;
	}

	bool EditorCameraController::OnMouseButtonReleased(Engine::MouseButtonReleasedEvent& e)
	{
		if (e.GetMouseButton() == Engine::MouseCode::ButtonRight)
		{
			m_RotateDragging = false;
		}
		else if (e.GetMouseButton() == Engine::MouseCode::ButtonMiddle)
		{
			m_PanDragging = false;
		}
		return false;
	}

	bool EditorCameraController::OnMouseMoved(Engine::MouseMovedEvent& e)
	{
		if (!m_RotateDragging && !m_PanDragging) return false;

		glm::vec2 mouse = { e.GetX(), e.GetY() };
		glm::vec2 delta = (mouse - m_StartMousePosition) * m_MouseSensitivity;
		m_StartMousePosition = mouse;

		if (m_RotateDragging)
		{
			float yaw = m_Camera->GetYaw() + delta.x;
			float pitch = m_Camera->GetPitch() + delta.y;
			m_Camera->SetRotation(pitch, yaw);

		}
		else if (m_PanDragging)
		{
			glm::vec3 position = m_Camera->GetPosition();
			glm::vec3 right = m_Camera->GetRight();
			glm::vec3 up = m_Camera->GetUp();
			position -= right * delta.x * m_PanSpeed;
			position += up * delta.y * m_PanSpeed;
			m_Camera->SetPosition(position);
		}
		return false;
	}

	void EditorCameraController::FocusOnPoint(const glm::vec3& point, float distance)
	{
		glm::vec3 position = point - m_Camera->GetForward() * distance;
		m_Camera->SetPosition(position);
	}
}