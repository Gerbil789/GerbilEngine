#ifndef DIST
#define WEBGPU_CPP_IMPLEMENTATION
#endif

#include "EditorApp.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Core/EditorWindowManager.h"
#include "Editor/Core/IconManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Utility/FileWatcher.h"

#include "Engine/Core/Time.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Runtime.h"

#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Audio/Audio.h"

#include "Engine/Event/EventBus.h"
#include "Engine/Event/WindowEvent.h"

#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/Renderer.h"

#include "Engine/Physics/Physics.h"

#include "Engine/Debug/RenderDoc.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/TransformSystem.h"

#include "Engine/Core/Resources.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Configuration.h"

namespace Editor
{
	EditorApp::EditorApp()
	{
		//RenderDoc::Initialize(); //TODO: enable/disable at runtime in menu bar
		EditorSettings::Load();
		Engine::Project::Load(EditorSettings::projectDirectory);
		const Engine::Project& project = Engine::Project::GetActive();

		Engine::GraphicsContext::Initialize();
		GLFW::Initialize();

		m_Window.Initialize({ std::format("Gerbil Editor - {}", Engine::Configuration) , 1600, 900, "Resources/Editor/icons/logo.png" });
		m_Window.SetEventCallback([](Engine::Event& e) {Engine::EventBus::Get().Publish(e); });

		Engine::AssetManager::Initialize(project.GetProjectDirectory());

		Engine::Input::SetActiveWindow(*m_Window.GetNativeWindow());
		Engine::g_Renderer.Initialize(); //TODO: i dont like global variable
		EditorCommandManager::Initialize();
		FileWatcher::WatchDirectory(project.GetAssetsDirectory());
		Engine::Audio::Initialize();
		IconManager::Initialize();
		EditorWindowManager::Initialize(m_Window);

		std::filesystem::path dllPath = project.GetProjectDirectory() / "bin/windows/" / Engine::Configuration / (project.GetTitle() + ".dll");
		Engine::Runtime::LoadScripts(dllPath);

		auto id = project.GetDefaultSceneId();

		if(!Engine::AssetManager::GetAssetRegistry().GetRecord(id).IsValid())
		{
			LOG_ERROR("Failed to load default scene '{}', loading empty scene instead!", id);
			id = RESOURCES::SCENE::DEFAULT;
		}

		Engine::Scene& scene = Engine::AssetManager::GetAsset<Engine::Scene>(id);

		Engine::Scene& newScene = Engine::AssetManager::CreateAsset<Engine::Scene>();
		newScene = scene;


		Engine::SceneManager::SetActiveScene(newScene.id);
		EditorCommandManager::SetContext(&newScene);

		Engine::EventBus::Get().Subscribe<Engine::WindowCloseEvent>([this](auto&) {m_Running = false; LOG_INFO("Application closed"); });
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

			Engine::Time::BeginFrame();				// update delta time and FPS counters
			Engine::Input::Update();					// poll input events
			Engine::Audio::Update();					// release finished audio voices back to pool
			Engine::TransformSystem::Update();

			EditorWindowManager::Update();		// update editor UI, render viewport, ...
			EditorCommandManager::Flush();		// execute queued commands (deffered execution)
			
			if (EditorContext::state == EditorState::Play)
			{
				Engine::PhysicsSystem::Update();
				Engine::Runtime::Update();			// update game runtime (scripts, audio listener, etc...)
			}
		}
	}
}