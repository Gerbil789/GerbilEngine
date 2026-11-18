#include "editor/editor.h"
#include "engine/core/log.h"
#include "editor/core/editor_context.h"
#include "editor/core/editor_renderer.h"
#include "editor/core/editor_command_manager.h"
#include "engine/system/camera_system.h"

namespace editor
{
	Editor::Editor(const engine::Application& app) : m_app(app)
	{
		EditorRenderer::Initialize(m_app.s_window);

		editor::context::camera.backgroundMode = engine::component::Camera::Background::Skybox;
		editor::context::camera.projection = engine::component::Camera::Projection::Perspective;
		editor::context::cameraTransform.position = glm::vec3{ 0.0f, 0.0f, -20.0f };
		editor::context::cameraTransform.rotation = glm::vec3{ 0.0f, glm::radians(180.0f), 0.0f };
		engine::Camerasystem::UpdateCameraViewMatrix(editor::context::camera, editor::context::cameraTransform);

		LOG_INFO("--- editor initialization complete ---");
	}

	Editor::~Editor()
	{
		EditorRenderer::Shutdown();
	}

	void Editor::Update()
	{
		m_app.Update();

		EditorRenderer::Update();
		EditorCommandManager::ExecuteDeferred();
		editor::context::UpdateSelection();
	}
}