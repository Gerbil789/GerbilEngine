#include "Editor/Editor.h"
#include "Engine/Core/Log.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Core/EditorRenderer.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Engine/System/CameraSystem.h"

namespace editor
{
	Editor::Editor(const engine::Application& app) : m_app(app)
	{
		EditorRenderer::Initialize(m_app.s_window);

		editor::context::camera.backgroundMode = engine::component::CameraComponent::Background::Skybox;
		editor::context::camera.projection = engine::component::CameraComponent::Projection::Perspective;
		editor::context::cameraTransform.position = glm::vec3{ 0.0f, 0.0f, -20.0f };
		editor::context::cameraTransform.rotation = glm::vec3{ 0.0f, glm::radians(180.0f), 0.0f };
		engine::CameraSystem::UpdateCameraViewMatrix(editor::context::camera, editor::context::cameraTransform);

		LOG_INFO("--- Editor initialization complete ---");
	}

	Editor::~Editor()
	{
		EditorRenderer::Shutdown();
	}

	void Editor::Update()
	{
		EditorCommandManager::Update();
		m_app.Update();

		EditorRenderer::Update();
		EditorCommandManager::ExecuteDeferredCommands();
		editor::context::UpdateSelection();
	}
}