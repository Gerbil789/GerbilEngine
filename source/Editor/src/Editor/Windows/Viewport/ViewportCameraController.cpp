#include "ViewportCameraController.h"
#include "Editor/Core/EditorContext.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/System/CameraSystem.h"

namespace editor
{
	namespace
	{
		glm::vec2 s_startMousePosition = { 0.0f, 0.0f };
		bool s_rotateDragging = false;
		bool s_panDragging = false;

		float s_mouseDragSensitivity = 0.12f;
		float s_mouseRotateSensitivity = 0.002f;
		float s_scrollSensitivity = 1.0f;
		float s_panSpeed = 0.1f;
		float s_focusDistance = 10.0f;
	}

	void ViewportCameraController::Update()
	{
		if (editor::context::editorMode != EditorMode::Edit) return;

		if (engine::input::IsMouseButtonPressed(engine::Mouse::ButtonRight)) { s_rotateDragging = true; }
		if (engine::input::IsMouseButtonPressed(engine::Mouse::ButtonMiddle)) { s_panDragging = true; }
		if (engine::input::IsMouseButtonReleased(engine::Mouse::ButtonRight)) { s_rotateDragging = false; }
		if (engine::input::IsMouseButtonReleased(engine::Mouse::ButtonMiddle)) { s_panDragging = false; }

		glm::vec2 mousePosition = engine::input::GetMousePosition();
		auto& tc = editor::context::cameraTransform;

		if(s_rotateDragging)
		{
			glm::vec2 delta = (mousePosition - s_startMousePosition) * s_mouseRotateSensitivity;

			float yaw = tc.rotation.y + delta.x;
			float pitch = tc.rotation.x - delta.y;
			tc.rotation = { pitch, yaw, 0.0f };
		}
		else if(s_panDragging)
		{
			glm::vec2 delta = (mousePosition - s_startMousePosition) * s_mouseDragSensitivity;
			glm::vec3 right = engine::CameraSystem::GetRight(tc);
			glm::vec3 up = engine::CameraSystem::GetUp(tc);
			tc.position += right * delta.x * s_panSpeed;
			tc.position += up * delta.y * s_panSpeed;
		}

		s_startMousePosition = mousePosition;
		glm::vec2 mouseScroll = engine::input::GetMouseScroll();

		if(mouseScroll.y != 0.0f)
		{
			float delta = mouseScroll.y * s_scrollSensitivity;
			glm::vec3 forward = engine::CameraSystem::GetForward(tc);
			tc.position -= forward * delta;
		}

		if(engine::input::IsKeyPressed(engine::Key::F) && editor::context::selectedEntity != entt::null)
		{
			entt::registry& registry = engine::SceneManager::GetActiveScene().Get().registry;

			auto transform = registry.try_get<engine::component::Transform>(editor::context::selectedEntity);
			if(!transform) return;

			glm::vec3 focusPoint = transform->position;
			glm::vec3 forward = engine::CameraSystem::GetForward(tc);
			tc.position = focusPoint + forward * s_focusDistance;
		}

		engine::CameraSystem::UpdateCameraViewMatrix(editor::context::camera, tc);
	}
}