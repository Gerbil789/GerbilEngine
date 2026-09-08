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
#include "Editor/Core/SelectionManager.h"
#include "Engine/System/CameraSystem.h"
// #include "Engine/Core/Runtime.h"

namespace editor
{
	Editor::Editor(const engine::Application::Configuration& config) : Application(config)
	{
		EditorCommandManager::Initialize();
		EditorRenderer::Initialize(Application::s_Window);
		SelectionManager::Initialize();

		editor::editorContext.camera.backgroundMode = engine::CameraComponent::Background::Skybox;
		editor::editorContext.camera.projectionType = engine::CameraComponent::Projection::Perspective;
		editor::editorContext.cameraTransform.position = glm::vec3{ 0.0f, 0.0f, -20.0f };
		editor::editorContext.cameraTransform.rotation = glm::vec3{ 0.0f, glm::radians(180.0f), 0.0f };
		engine::CameraSystem::UpdateCameraViewMatrix(editor::editorContext.camera, editor::editorContext.cameraTransform);

		LOG_INFO("--- Editor initialization complete ---");
	}

	Editor::~Editor()
	{
		EditorRenderer::Shutdown();
		Application::~Application();
	}

	void Editor::Run()
	{
		while(Application::s_Running)
		{
			Update();
		}
	}

	void Editor::Update()
	{
		Application::Update();

		EditorRenderer::Update();
		EditorCommandManager::ExecuteDeferredCommands();

		//if (editorContext.editorMode == EditorMode::Play)
		//{
		//	Engine::Runtime::Update();
		//}
	}


}