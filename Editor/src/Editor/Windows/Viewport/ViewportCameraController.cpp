#include "ViewportCameraController.h"
#include "Editor/Core/EditorEvent.h"
#include "Editor/Core/EditorContext.h"
#include "Engine/Core/Input.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include "Engine/Asset/AssetManager.h"

namespace Editor
{
	namespace
	{
		glm::vec2 m_StartMousePosition = { 0.0f, 0.0f };
		bool m_RotateDragging = false;
		bool m_PanDragging = false;

		float m_MouseSensitivity = 0.12f;
		float m_ScrollSensitivity = 1.0f;
		float m_PanSpeed = 0.1f;
	}

	void ViewportCameraController::Initialize()
	{
		Engine::EventBus::Subscribe<Engine::MouseScrolledEvent>([this](const auto& e) {OnMouseScroll(e); return false; });
		Engine::EventBus::Subscribe<Engine::MouseButtonPressedEvent>([this](const auto& e) {OnMouseButtonPressed(e); return false; });
		Engine::EventBus::Subscribe<Engine::MouseButtonReleasedEvent>([this](const auto& e) {OnMouseButtonReleased(e); return false; });
		Engine::EventBus::Subscribe<Engine::MouseMovedEvent>([this](const auto& e) {OnMouseMoved(e); return false; });
		Engine::EventBus::Subscribe<FocusEntityEvent>([this](const FocusEntityEvent& e) {OnEntityFocus(e.id); return false; });
	}

	void ViewportCameraController::OnMouseScroll(const Engine::MouseScrolledEvent& e)
	{
		if (!m_ViewportHovered) return;

		float delta = static_cast<float>(e.yOffset) * m_ScrollSensitivity;
		glm::vec3 position = EditorContext::editorCamera.GetPosition();
		EditorContext::editorCamera.SetPosition(position + EditorContext::editorCamera.GetForward() * delta * m_ScrollSensitivity);
	}

	void ViewportCameraController::OnMouseButtonPressed(const Engine::MouseButtonPressedEvent& e)
	{
		if (!m_ViewportHovered) return;

		if (e.button == Engine::Mouse::ButtonRight)
		{
			m_RotateDragging = true;
			m_StartMousePosition = Engine::Input::GetMousePosition();
		}
		else if (e.button == Engine::Mouse::ButtonMiddle)
		{
			m_PanDragging = true;
			m_StartMousePosition = Engine::Input::GetMousePosition();
		}
	}

	void ViewportCameraController::OnMouseButtonReleased(const Engine::MouseButtonReleasedEvent& e)
	{
		if (e.button == Engine::Mouse::ButtonRight)
		{
			m_RotateDragging = false;
		}
		else if (e.button == Engine::Mouse::ButtonMiddle)
		{
			m_PanDragging = false;
		}
	}

	void ViewportCameraController::OnMouseMoved(const Engine::MouseMovedEvent& e)
	{
		if (!m_RotateDragging && !m_PanDragging) return;

		glm::vec2 mouse = { e.x, e.y };
		glm::vec2 delta = (mouse - m_StartMousePosition) * m_MouseSensitivity;
		m_StartMousePosition = mouse;

		if (m_RotateDragging)
		{
			float yaw = EditorContext::editorCamera.GetYaw() + delta.x;
			float pitch = EditorContext::editorCamera.GetPitch() + delta.y;
			EditorContext::editorCamera.SetRotation(pitch, yaw);

		}
		else if (m_PanDragging)
		{
			glm::vec3 position = EditorContext::editorCamera.GetPosition();
			glm::vec3 right = EditorContext::editorCamera.GetRight();
			glm::vec3 up = EditorContext::editorCamera.GetUp();
			position -= right * delta.x * m_PanSpeed;
			position += up * delta.y * m_PanSpeed;
			EditorContext::editorCamera.SetPosition(position);
		}
	}

	void ViewportCameraController::OnEntityFocus(Engine::Uuid entityId, float distance)
	{
		Engine::Scene& scene = Engine::AssetManager::GetAsset<Engine::Scene>(Engine::SceneManager::GetActiveScene());
		entt::entity entity = scene.GetEntity(entityId);
		if (entity == entt::null) return;

		glm::vec3 focusPoint = scene.GetRegistry().get<Engine::TransformComponent>(entity).position;
		glm::vec3 position = focusPoint - EditorContext::editorCamera.GetForward() * distance;
		EditorContext::editorCamera.SetPosition(position);
	}
}