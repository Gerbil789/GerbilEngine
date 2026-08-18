#ifndef DIST
#define WEBGPU_CPP_IMPLEMENTATION //TODO: handle macros in premake
#endif

#include "EditorApp.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Core/EditorWindowManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Utility/FileWatcher.h"
#include "Editor/Core/SelectionManager.h"

#include "Engine/Core/Log.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/Configuration.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/WindowEvent.h"
#include "Engine/Event/ApplicationEvent.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Debug/RenderDoc.h"
#include "Editor/Core/EditorState.h"
#include "Engine/System/TransformSystem.h"
#include "Engine/Graphics/Font.h"

namespace Editor
{
	namespace
	{
		Engine::Window m_Window;
		bool m_Running = true;
	}

	EditorApp::EditorApp()
	{
		//RenderDoc::Initialize(); //TODO: enable/disable at runtime in menu bar

		Engine::Project::Load(Editor::GetProjectPath());
		const Engine::Project& project = Engine::Project::GetActive();

		Engine::GraphicsContext::Initialize();
		GLFW::Initialize();

		m_Window.Initialize({ std::format("Gerbil Editor - {}", Engine::Configuration) , 1600, 900, "resources/icons/logo.png" });
		
		m_Window.SetEventCallback([](auto& e) {Engine::EventBus::Publish(e); });

		Engine::AssetManager::Initialize(project.GetProjectDirectory());

		Engine::Input::SetActiveWindow(*m_Window.GetNativeWindow());
		Editor::editorContext.renderer.Initialize();
		Editor::editorContext.renderer.SetFlags(Engine::RenderPassType::Background | Engine::RenderPassType::Shadow | Engine::RenderPassType::Opaque | Engine::RenderPassType::UI/* | Engine::RenderPassType::Normal | Engine::RenderPassType::Wireframe*/);
		EditorCommandManager::Initialize();
		FileWatcher::WatchDirectory(project.GetAssetsDirectory());
		Engine::Audio::Initialize();
		EditorWindowManager::Initialize(m_Window);
		SelectionManager::Initialize();
		Engine::FontManager::Initialize();

		Engine::EventBus::Subscribe<Engine::SceneChangedEvent>([this](auto& e)
			{
				const std::string& name = Engine::AssetManager::GetAssetPath(e.id).stem().string();
				m_Window.SetTitle(std::format("Gerbil Editor - {} - Scene: {}", Engine::Configuration, name));
				return false;
			});

		std::filesystem::path dllPath = project.GetProjectDirectory() / "bin/windows/" / Engine::Configuration / (project.GetTitle() + ".dll");
		Engine::Runtime::LoadScripts(dllPath);

		Engine::Uuid id = project.GetDefaultSceneId();
		Engine::SceneManager::SetActiveScene(id);


		Engine::EventBus::Subscribe<Engine::WindowCloseEvent>([this](auto&) {m_Running = false; LOG_INFO("Application closed"); return false; });
		LOG_INFO("--- Editor initialization complete ---");
	}

	EditorApp::~EditorApp()
	{
		FileWatcher::Shutdown();
		Engine::Audio::Shutdown();
		EditorWindowManager::Shutdown();
		m_Window.Shutdown();
		GLFW::Shutdown();
		Engine::GraphicsContext::Shutdown();
	}

	void EditorApp::Run()
	{
		while (m_Running)
		{
			if (m_Window.IsMinimized())
			{
				GLFW::WaitEvents();
				Engine::Time::BeginFrame();
				continue;
			}

			Engine::Time::BeginFrame();
			Engine::Input::Update();
			Engine::Audio::Update();

			Engine::Scene& scene = Engine::AssetManager::GetAsset<Engine::Scene>(Engine::SceneManager::GetActiveScene());

			Engine::TransformSystem::Update(scene);

			EditorWindowManager::Update();
			EditorCommandManager::ExecuteDeferredCommands();

			if (Editor::editorContext.editorMode == EditorMode::Play)
			{
				Engine::Runtime::Update();
			}
		}
	}
}