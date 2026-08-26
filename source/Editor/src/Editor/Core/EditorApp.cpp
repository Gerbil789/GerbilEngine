#include "Editor/Core/EditorApp.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Core/EditorWindowManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Core/SelectionManager.h"
#include "Editor/Core/EditorState.h"

#include "Engine/Core/Log.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/Configuration.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/System/TransformSystem.h"

namespace Editor
{
	void EditorApp::Initialize(const Engine::Window& window)
	{
		Editor::editorContext.camera.background = Engine::CameraComponent::Background::Skybox;
		Editor::editorContext.camera.projectionType = Engine::CameraComponent::Projection::Perspective;
		Editor::editorContext.cameraTransform.position = glm::vec3{ 0.0f, 0.0f, -20.0f };

		Editor::editorContext.renderer.Initialize();
		Editor::editorContext.renderer.SetFlags(Engine::RenderPassType::Background | Engine::RenderPassType::Shadow | Engine::RenderPassType::Opaque | Engine::RenderPassType::UI/* | Engine::RenderPassType::Normal | Engine::RenderPassType::Wireframe*/);
		Editor::editorContext.renderer.SetCamera(Editor::editorContext.camera, Editor::editorContext.cameraTransform);

		EditorCommandManager::Initialize();
		EditorWindowManager::Initialize(window);
		SelectionManager::Initialize();

		LOG_INFO("--- Editor initialization complete ---");
	}

	void EditorApp::Shutdown()
	{
		EditorWindowManager::Shutdown();
	}

	void EditorApp::Update()
	{
		EditorWindowManager::Update();
		EditorCommandManager::ExecuteDeferredCommands();

		if (Editor::editorContext.editorMode == EditorMode::Play)
		{
			Engine::Runtime::Update();
		}
	}
}