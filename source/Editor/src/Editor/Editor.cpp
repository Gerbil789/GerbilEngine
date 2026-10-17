#include "Editor/Editor.h"
#include "Engine/Core/Project.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Input.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Graphics/Font.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Event/WindowEvent.h"
#include "Engine/Core/Time.h"
#include "Engine/System/TransformSystem.h"
#include "Engine/Core/Log.h"
#include "Engine/Utility/Path.h"

#include "Editor/Core/EditorContext.h"
#include "Editor/Core/EditorRenderer.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Engine/System/CameraSystem.h"

namespace editor
{
	Editor::Editor(const engine::Application& app) : m_App(app)
	{
		EditorCommandManager::Initialize();
		EditorRenderer::Initialize(m_App.s_Window);
		//SelectionManager::Initialize();

		editorContext.camera.backgroundMode = engine::CameraComponent::Background::Skybox;
		editorContext.camera.projectionType = engine::CameraComponent::Projection::Perspective;
		editorContext.cameraTransform.position = glm::vec3{ 0.0f, 0.0f, -20.0f };
		editorContext.cameraTransform.rotation = glm::vec3{ 0.0f, glm::radians(180.0f), 0.0f };
		engine::CameraSystem::UpdateCameraViewMatrix(editor::editorContext.camera, editor::editorContext.cameraTransform);

		LOG_INFO("--- Editor initialization complete ---");
	}

	Editor::~Editor()
	{
		EditorRenderer::Shutdown();
	}

	void Editor::Update()
	{
		m_App.Update();

		EditorRenderer::Update();
		EditorCommandManager::ExecuteDeferredCommands();

		//if (editorContext.editorMode == EditorMode::Play)
		//{
		//	Engine::Runtime::Update();
		//}
	}


}