#include "ViewportCameraController.h"
#include "Engine/Core/Input.h"
#include "Engine/Scene/Entity.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Event/KeyEvent.h"

namespace Editor
{
	void ViewportCameraController::Initialize(Engine::Camera* camera)
	{
		m_Camera = camera;
		Engine::EventBus::Get().Subscribe<Engine::KeyPressedEvent>([this](auto e) {OnKeyPressed(e); });
		Engine::EventBus::Get().Subscribe<Engine::MouseScrolledEvent>([this](auto e) {OnMouseScroll(e); });
		Engine::EventBus::Get().Subscribe<Engine::MouseButtonPressedEvent>([this](auto e) {OnMouseButtonPressed(e); });
		Engine::EventBus::Get().Subscribe<Engine::MouseButtonReleasedEvent>([this](auto e) {OnMouseButtonReleased(e); });
		Engine::EventBus::Get().Subscribe<Engine::MouseMovedEvent>([this](auto e) {OnMouseMoved(e); });
	}

	void ViewportCameraController::OnKeyPressed(Engine::KeyPressedEvent& e)
	{
		if (e.GetKey() == Engine::KeyCode::F) //Focus
		{
			auto entity = Engine::SceneManager::GetActiveScene().GetEntity(SelectionManager::GetPrimary(SelectionType::Entity));
			if (!entity) return;

			glm::vec3 focusPoint = entity.Get<Engine::TransformComponent>().position;
			FocusOnPoint(focusPoint);
		}
	}

	void ViewportCameraController::OnMouseScroll(Engine::MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * m_ScrollSensitivity;
		glm::vec3 position = m_Camera->GetPosition();
		m_Camera->SetPosition(position + m_Camera->GetForward() * delta * m_ScrollSensitivity);
	}

	void ViewportCameraController::OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e)
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
	}

	void ViewportCameraController::OnMouseButtonReleased(Engine::MouseButtonReleasedEvent& e)
	{
		if (e.GetMouseButton() == Engine::MouseCode::ButtonRight)
		{
			m_RotateDragging = false;
		}
		else if (e.GetMouseButton() == Engine::MouseCode::ButtonMiddle)
		{
			m_PanDragging = false;
		}
	}

	void ViewportCameraController::OnMouseMoved(Engine::MouseMovedEvent& e)
	{
		if (!m_RotateDragging && !m_PanDragging) return;

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
	}

	void ViewportCameraController::FocusOnPoint(const glm::vec3& point, float distance)
	{
		glm::vec3 position = point - m_Camera->GetForward() * distance;
		m_Camera->SetPosition(position);
	}
}