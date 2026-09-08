#include "ViewportCameraController.h"
#include "Editor/Core/EditorEvent.h"
#include "Editor/Core/EditorContext.h"
#include "Engine/Core/Input.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/System/CameraSystem.h"
#include "Engine/System/TransformSystem.h"

namespace editor
{
	namespace
	{
		glm::vec2 m_StartMousePosition = { 0.0f, 0.0f };
		bool m_RotateDragging = false;
		bool m_PanDragging = false;

		float m_MouseDragSensitivity = 0.12f;
		float m_MouseRotateSensitivity = 0.002f;
		float m_ScrollSensitivity = 1.0f;
		float m_PanSpeed = 0.1f;
	}

	void ViewportCameraController::Initialize()
	{
		engine::EventBus::Subscribe<engine::MouseScrolledEvent>([this](const auto& e) {OnMouseScroll(e); return false; });
		engine::EventBus::Subscribe<engine::MouseButtonPressedEvent>([this](const auto& e) {OnMouseButtonPressed(e); return false; });
		engine::EventBus::Subscribe<engine::MouseButtonReleasedEvent>([this](const auto& e) {OnMouseButtonReleased(e); return false; });
		engine::EventBus::Subscribe<engine::MouseMovedEvent>([this](const auto& e) {OnMouseMoved(e); return false; });
		engine::EventBus::Subscribe<FocusEntityEvent>([this](const auto& e) {OnEntityFocus(e.id); return false; });
	}

	void ViewportCameraController::OnMouseScroll(const engine::MouseScrolledEvent& e)
	{
		if (!m_ViewportHovered) return;

		if (editor::editorContext.editorMode != EditorMode::Edit) return;

		auto& tc = editor::editorContext.cameraTransform;

		float delta = static_cast<float>(e.yOffset) * m_ScrollSensitivity;
		glm::vec3 forward = engine::CameraSystem::GetForward(tc);

		tc.position -= forward * delta;

		const glm::mat4 localMatrix = engine::TransformSystem::CalculateLocalPositionMatrix(tc);
		engine::CameraSystem::UpdateCameraViewMatrix(editor::editorContext.camera, tc);
	}

	void ViewportCameraController::OnMouseButtonPressed(const engine::MouseButtonPressedEvent& e)
	{
		if (!m_ViewportHovered) return;
		if (editor::editorContext.editorMode != EditorMode::Edit) return;

		if (e.button == engine::Mouse::ButtonRight)
		{
			m_RotateDragging = true;
			m_StartMousePosition = engine::Input::GetMousePosition();
		}
		else if (e.button == engine::Mouse::ButtonMiddle)
		{
			m_PanDragging = true;
			m_StartMousePosition = engine::Input::GetMousePosition();
		}
	}

	void ViewportCameraController::OnMouseButtonReleased(const engine::MouseButtonReleasedEvent& e)
	{
		if (editor::editorContext.editorMode != EditorMode::Edit) return;

		if (e.button == engine::Mouse::ButtonRight)
		{
			m_RotateDragging = false;
		}
		else if (e.button == engine::Mouse::ButtonMiddle)
		{
			m_PanDragging = false;
		}
	}

	void ViewportCameraController::OnMouseMoved(const engine::MouseMovedEvent& e)
	{
		if (editor::editorContext.editorMode != EditorMode::Edit) return;
		if (!m_RotateDragging && !m_PanDragging) return;

		auto& tc = editor::editorContext.cameraTransform;

		glm::vec2 mouse = { e.x, e.y };

		if (m_RotateDragging)
		{
			glm::vec2 delta = (mouse - m_StartMousePosition) * m_MouseRotateSensitivity;

			float yaw = tc.rotation.y + delta.x;
			float pitch = tc.rotation.x - delta.y;
			tc.rotation = { pitch, yaw, 0.0f };
		}
		else if (m_PanDragging)
		{
			glm::vec2 delta = (mouse - m_StartMousePosition) * m_MouseDragSensitivity;
			glm::vec3 right = engine::CameraSystem::GetRight(tc);
			glm::vec3 up = engine::CameraSystem::GetUp(tc);
			tc.position += right * delta.x * m_PanSpeed;
			tc.position += up * delta.y * m_PanSpeed;
		}

		m_StartMousePosition = mouse;

		engine::CameraSystem::UpdateCameraViewMatrix(editor::editorContext.camera, tc);
	}

	void ViewportCameraController::OnEntityFocus(engine::Uuid entityId, float distance)
	{
		if (editor::editorContext.editorMode != EditorMode::Edit) return;
		if (!entityId) return;

		engine::SceneAsset& scene = engine::AssetManager::GetAsset(engine::SceneManager::GetActiveScene());
		engine::Entity entity = scene.GetEntity(entityId);
		if (!entity) return;
		if (!entity.HasComponent<engine::TransformComponent>()) return;

		auto& tc = editor::editorContext.cameraTransform;
		glm::vec3 forward = engine::CameraSystem::GetForward(tc);

		glm::vec3 focusPoint = entity.GetComponent<engine::TransformComponent>().position;
		tc.position = focusPoint - forward * distance;

		engine::CameraSystem::UpdateCameraViewMatrix(editor::editorContext.camera, tc);
	}
}