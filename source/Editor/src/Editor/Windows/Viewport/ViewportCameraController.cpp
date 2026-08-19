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

namespace Editor
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
		Engine::EventBus::Subscribe<Engine::MouseScrolledEvent>([this](const auto& e) {OnMouseScroll(e); return false; });
		Engine::EventBus::Subscribe<Engine::MouseButtonPressedEvent>([this](const auto& e) {OnMouseButtonPressed(e); return false; });
		Engine::EventBus::Subscribe<Engine::MouseButtonReleasedEvent>([this](const auto& e) {OnMouseButtonReleased(e); return false; });
		Engine::EventBus::Subscribe<Engine::MouseMovedEvent>([this](const auto& e) {OnMouseMoved(e); return false; });
		Engine::EventBus::Subscribe<FocusEntityEvent>([this](const auto& e) {OnEntityFocus(e.id); return false; });
	}

	void ViewportCameraController::OnMouseScroll(const Engine::MouseScrolledEvent& e)
	{
		if (!m_ViewportHovered) return;

		if (Editor::editorContext.editorMode != EditorMode::Edit) return;

		Engine::SceneAsset& scene = Engine::AssetManager::GetAsset<Engine::SceneAsset>(Engine::SceneManager::GetActiveScene());
		entt::entity cameraEntity = scene.GetActiveCamera();
		if (cameraEntity == entt::null) return;

		auto& tc = scene.GetRegistry().get<Engine::TransformComponent>(cameraEntity);

		float delta = static_cast<float>(e.yOffset) * m_ScrollSensitivity;
		glm::vec3 forward = Engine::CameraSystem::GetForward(tc);

		tc.position += forward * delta;

		scene.GetRegistry().emplace_or_replace<Engine::TransformDirty>(cameraEntity);
		scene.GetRegistry().emplace_or_replace<Engine::CameraViewDirty>(cameraEntity);
	}

	void ViewportCameraController::OnMouseButtonPressed(const Engine::MouseButtonPressedEvent& e)
	{
		if (!m_ViewportHovered) return;
		if (Editor::editorContext.editorMode != EditorMode::Edit) return;

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
		if (Editor::editorContext.editorMode != EditorMode::Edit) return;

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
		if (Editor::editorContext.editorMode != EditorMode::Edit) return;
		if (!m_RotateDragging && !m_PanDragging) return;

		Engine::SceneAsset& scene = Engine::AssetManager::GetAsset<Engine::SceneAsset>(Engine::SceneManager::GetActiveScene());
		entt::entity cameraEntity = scene.GetActiveCamera();
		if (cameraEntity == entt::null) return;

		auto& tc = scene.GetRegistry().get<Engine::TransformComponent>(cameraEntity);

		glm::vec2 mouse = { e.x, e.y };

		if (m_RotateDragging)
		{
			glm::vec2 delta = (mouse - m_StartMousePosition) * m_MouseRotateSensitivity;

			float yaw = tc.rotation.y+ delta.x;
			float pitch = tc.rotation.x + delta.y;
			tc.rotation = { pitch, yaw, 0.0f };
		}
		else if (m_PanDragging)
		{
			glm::vec2 delta = (mouse - m_StartMousePosition) * m_MouseDragSensitivity;

			glm::vec3 right = Engine::CameraSystem::GetRight(tc);
			glm::vec3 up = Engine::CameraSystem::GetUp(tc);
			tc.position -= right * delta.x * m_PanSpeed;
			tc.position += up * delta.y * m_PanSpeed;
		}

		m_StartMousePosition = mouse;

		scene.GetRegistry().emplace_or_replace<Engine::CameraViewDirty>(cameraEntity);
		scene.GetRegistry().emplace_or_replace<Engine::TransformDirty>(cameraEntity);
	}

	void ViewportCameraController::OnEntityFocus(Engine::Uuid entityId, float distance)
	{
		if (Editor::editorContext.editorMode != EditorMode::Edit) return;
		if (!entityId) return;

		Engine::SceneAsset& scene = Engine::AssetManager::GetAsset<Engine::SceneAsset>(Engine::SceneManager::GetActiveScene());
		Engine::Entity entity = scene.GetEntity(entityId);
		if (!entity) return;
		if (!entity.HasComponent<Engine::TransformComponent>()) return;

		entt::entity cameraEntity = scene.GetActiveCamera();
		if (cameraEntity == entt::null) return;

		auto& tc = scene.GetRegistry().get<Engine::TransformComponent>(cameraEntity);
		glm::vec3 forward = Engine::CameraSystem::GetForward(tc);

		glm::vec3 focusPoint = entity.GetComponent<Engine::TransformComponent>().position;
		tc.position = focusPoint - forward * distance;

		scene.GetRegistry().emplace_or_replace<Engine::CameraViewDirty>(cameraEntity);
		scene.GetRegistry().emplace_or_replace<Engine::CameraProjectionDirty>(cameraEntity);
		scene.GetRegistry().emplace_or_replace<Engine::TransformDirty>(cameraEntity);
	}
}